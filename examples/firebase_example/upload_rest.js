// بسم الله الرحمن الرحيم
const fs = require('fs');
const path = require('path');
const https = require('https');

const PROJECT_ID = 'slangserver-f3fd0';
const BUCKET_NAME = 'slangserver-f3fd0.appspot.com';
const PACKAGE_NAME = 'firebase-example';
const VERSION = '1.0.0';

// Files to upload
const files = [
    'sad.toml',
    'main.s',
    'firebase.sad',
    'package_registry.s',
    'README.md'
];

async function uploadToFirebase(filePath, destination) {
    return new Promise((resolve, reject) => {
        const fileContent = fs.readFileSync(filePath);
        const fileName = path.basename(filePath);
        
        const url = `https://www.googleapis.com/upload/storage/v1/b/${BUCKET_NAME}/o?uploadType=media&name=${encodeURIComponent(destination)}`;
        
        const options = {
            hostname: 'www.googleapis.com',
            port: 443,
            path: `/upload/storage/v1/b/${BUCKET_NAME}/o?uploadType=media&name=${encodeURIComponent(destination)}`,
            method: 'POST',
            headers: {
                'Content-Length': fileContent.length,
                'Content-Type': 'application/octet-stream'
            }
        };
        
        console.log(`Uploading: ${fileName} to ${destination}`);
        
        const req = https.request(options, (res) => {
            let data = '';
            res.on('data', (chunk) => { data += chunk; });
            res.on('end', () => {
                if (res.statusCode === 200 || res.statusCode === 201) {
                    console.log(`✅ Success: ${fileName}`);
                    resolve(true);
                } else {
                    console.log(`⚠️  ${fileName}: ${res.statusCode}`);
                    resolve(false);
                }
            });
        });
        
        req.on('error', (error) => {
            console.log(`⚠️  ${fileName}: No auth available`);
            resolve(false);
        });
        
        req.write(fileContent);
        req.end();
    });
}

async function main() {
    console.log('📦 Firebase Example Package Uploader');
    console.log('====================================\n');
    console.log(`Project: ${PROJECT_ID}`);
    console.log(`Package: ${PACKAGE_NAME}@${VERSION}`);
    console.log(`Bucket: ${BUCKET_NAME}\n`);
    
    let uploaded = 0;
    
    for (const file of files) {
        const filePath = path.join(__dirname, file);
        const destination = `packages/${PACKAGE_NAME}/${VERSION}/${file}`;
        
        if (fs.existsSync(filePath)) {
            const success = await uploadToFirebase(filePath, destination);
            if (success) uploaded++;
            
            // Small delay between requests
            await new Promise(resolve => setTimeout(resolve, 500));
        } else {
            console.log(`⚠️  File not found: ${file}`);
        }
    }
    
    console.log(`\n📊 Upload Results: ${uploaded}/${files.length} files`);
    console.log(`\n🔗 Firebase Console:`);
    console.log(`https://console.firebase.google.com/u/0/project/${PROJECT_ID}/storage`);
    console.log(`\nFiles created in: packages/${PACKAGE_NAME}/${VERSION}/\n`);
}

main().catch(console.error);
