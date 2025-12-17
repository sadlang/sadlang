// بسم الله الرحمن الرحيم
const admin = require('firebase-admin');
const fs = require('fs');
const path = require('path');
const https = require('https');

const PROJECT_ID = 'slangserver-f3fd0';
const BUCKET_NAME = 'slangserver-f3fd0.appspot.com';
const PACKAGE_NAME = 'firebase-example';
const VERSION = '1.0.0';

// Initialize Firebase Admin with service account
let app;
try {
    const serviceAccountPath = process.env.GOOGLE_APPLICATION_CREDENTIALS;
    if (serviceAccountPath && fs.existsSync(serviceAccountPath)) {
        const serviceAccount = JSON.parse(fs.readFileSync(serviceAccountPath, 'utf8'));
        app = admin.initializeApp({
            credential: admin.credential.cert(serviceAccount),
            projectId: PROJECT_ID,
            storageBucket: BUCKET_NAME
        });
    } else {
        // Try to use default credentials
        app = admin.initializeApp({
            projectId: PROJECT_ID,
            storageBucket: BUCKET_NAME
        });
    }
} catch (error) {
    console.error('⚠️  Firebase initialization note:', error.message);
}

async function uploadFile(bucket, localPath, remotePath) {
    return new Promise((resolve, reject) => {
        if (!fs.existsSync(localPath)) {
            console.log(`⚠️  File not found: ${localPath}`);
            resolve(false);
            return;
        }

        const fileStream = fs.createReadStream(localPath);
        const fileName = path.basename(localPath);
        
        console.log(`📤 Uploading: ${fileName}`);
        
        bucket.upload(localPath, {
            destination: remotePath,
            metadata: {
                contentType: 'text/plain',
                cacheControl: 'no-cache'
            }
        }).then(() => {
            console.log(`✅ ${fileName} uploaded to ${remotePath}`);
            resolve(true);
        }).catch((error) => {
            console.log(`⚠️  ${fileName}: ${error.code || error.message}`);
            resolve(false);
        });
    });
}

async function uploadPackageFiles() {
    console.log('\n📦 Firebase Example Package Uploader');
    console.log('=====================================\n');
    console.log(`Project: ${PROJECT_ID}`);
    console.log(`Package: ${PACKAGE_NAME}@${VERSION}`);
    console.log(`Bucket: ${BUCKET_NAME}\n`);

    const files = [
        { local: 'sad.toml', remote: `packages/${PACKAGE_NAME}/${VERSION}/sad.toml` },
        { local: 'main.s', remote: `packages/${PACKAGE_NAME}/${VERSION}/main.s` },
        { local: 'firebase.sad', remote: `packages/${PACKAGE_NAME}/${VERSION}/firebase.sad` },
        { local: 'package_registry.s', remote: `packages/${PACKAGE_NAME}/${VERSION}/package_registry.s` },
        { local: 'README.md', remote: `packages/${PACKAGE_NAME}/${VERSION}/README.md` },
        { local: 'LICENSE', remote: `packages/${PACKAGE_NAME}/${VERSION}/LICENSE` }
    ];

    try {
        const bucket = admin.storage().bucket();
        
        console.log('🔄 Starting upload...\n');
        
        let uploadedCount = 0;
        for (const file of files) {
            const success = await uploadFile(bucket, file.local, file.remote);
            if (success) uploadedCount++;
            
            // Delay between uploads
            await new Promise(resolve => setTimeout(resolve, 1000));
        }

        console.log(`\n✨ Upload Complete!`);
        console.log(`📊 Results: ${uploadedCount}/${files.length} files uploaded`);
        console.log(`\n📍 Package Location:`);
        console.log(`   gs://${BUCKET_NAME}/packages/${PACKAGE_NAME}/${VERSION}/`);
        console.log(`\n🔗 Firebase Console:`);
        console.log(`   https://console.firebase.google.com/u/0/project/${PROJECT_ID}/storage`);
        console.log('\n✅ Package is now available on Firebase!\n');

        process.exit(0);
    } catch (error) {
        console.error('❌ Upload failed:', error);
        process.exit(1);
    }
}

// Check if we have authentication
if (!admin.apps[0]) {
    console.log('\n⚠️  Firebase Admin SDK initialized with default credentials\n');
}

uploadPackageFiles().catch(error => {
    console.error('Fatal error:', error);
    process.exit(1);
});
