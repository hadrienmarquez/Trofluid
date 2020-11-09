import express from 'express';
import path from 'path';
import {fileURLToPath} from 'url';
import {spawn} from 'child_process';

const __filename = fileURLToPath(import.meta.url);
const __dirname = path.dirname(__filename);

const router = express.Router();


router.post('/start', (req,res) =>{

    const _passPhrase = req.body.passphrase;
    
    console.log(_passPhrase);

    // Open ssh with communication with ffmpeg server and launch stream
    const cmd = spawn("ssh ffmpeg@trofluid_ffmpeg_1 bash -c '/usr/local/scripts/startStream.sh'", {shell: true});

    cmd.stdout.on('data', (data) => {
        console.log(`Stdout: ${data}`);
    });

    cmd.stderr.on('data', (data) => {
        console.log(`Stderr: ${data}`);
    });

    cmd.on('error', (error) => {
        console.log(`Error: ${error.messages}`);
    });

    cmd.on('close', (code) => {
        console.log(`Process exited with code ${code}`);
    });

    // res.json(testStartFluid);
});

router.get('/stop', (req,res) =>{
    
const testStopFluid = {
    msg: "Le button est connecté avec le serveur - STOP"
}

    // Open ssh with communication with ffmpeg server and stop stream
    const cmd = spawn("ssh ffmpeg@trofluid_ffmpeg_1 bash -c '/usr/local/scripts/stopStream.sh'", {shell: true});

    cmd.stdout.on('data', (data) => {
        console.log(`Stdout: ${data}`);
    });

    cmd.stderr.on('data', (data) => {
        console.log(`Stderr: ${data}`);
    });

    cmd.on('error', (error) => {
        console.log(`Error: ${error.messages}`);
    });

    cmd.on('close', (code) => {
        console.log(`Process exited with code ${code}`);
    });
});

export default router;