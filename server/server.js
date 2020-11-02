import express from 'express';
import path from 'path';
import {fileURLToPath} from 'url';
import router from './routes/api/trofluid.js'
import {exec} from 'child_process';

const __filename = fileURLToPath(import.meta.url);
const __dirname = path.dirname(__filename);

// APP SERVER STARTUP
const server = express();

const PORT =  process.env.PORT || 8080;

server.use(express.static(path.join(__dirname, 'public',)));

server.use(express.json());
server.use(express.urlencoded({extended: false}));

server.use('/api/trofluid', router);

server.listen(PORT, () => console.log(`Server started on port ${PORT}`));

// SSH STARTUP: Get RSA Fingerprint from server
exec('ssh-keyscan -H trofluid_ffmpeg_1 >> ~/.ssh/known_hosts', (error, stdout, stderr) => {
    if(error) {

        console.log(`Error: ${error}`);
        return;
    }

    if(stderr){
        console.log(`Stderr: ${stderr}`);
        return;
    }

    console.log(`Stdout: ${stdout}`);
}); 
