// بسم الله الرحمن الرحيم
const admin = require('firebase-admin');
const fs = require('fs');
const path = require('path');

// Initialize Firebase Admin
const serviceAccount = require('./firebase.json');

try {
    admin.initializeApp({
        projectId: 'slangserver-f3fd0',
        storageBucket: 'slangserver-f3fd0.appspot.com'
    });
} catch (e) {
    // Already initialized
}

const bucket = admin.storage().bucket();
const packageDir = 'packages/firebase-example/1.0.0';

// Files to upload
const files = [
    'sad.toml',
    'main.s',
    'firebase.sad',
    'package_registry.s',
    'README.md'
];

async function uploadFiles() {
    console.log('📦 Uploading firebase-example package to Firebase Storage...\n');
    
    let uploadedCount = 0;
    
    for (const file of files) {
        const filePath = path.join(__dirname, file);
        const destination = `${packageDir}/${file}`;
        
        if (fs.existsSync(filePath)) {
            try {
                await bucket.upload(filePath, {
                    destination: destination,
                    metadata: {
                        contentType: file.endsWith('.s') || file.endsWith('.sad') ? 'text/plain' : 'text/plain'
                    }
                });
                console.log(`✅ Uploaded: ${file} → ${destination}`);
                uploadedCount++;
            } catch (error) {
                console.error(`❌ Failed to upload ${file}: ${error.message}`);
            }
        } else {
            console.log(`⚠️  File not found: ${file}`);
        }
    }
    
    console.log(`\n✨ Upload complete! ${uploadedCount}/${files.length} files uploaded\n`);
    console.log(`📍 Package location: gs://slangserver-f3fd0.appspot.com/${packageDir}/`);
    console.log(`🔗 Firebase Console: https://console.firebase.google.com/u/0/project/slangserver-f3fd0/storage\n`);
    
    process.exit(0);
}

uploadFiles().catch(error => {
    console.error('Fatal error:', error);
    process.exit(1);
});
