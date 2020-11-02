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

    // Open ssh with communication with ffmpeg server
    const ls = spawn("ssh ffmpeg@trofluid_ffmpeg_1 bash -c '/usr/local/scripts/startStream.sh'", {shell: true});

    ls.stdout.on('data', (data) => {
        console.log(`Stdout: ${data}`);
    });

    ls.stderr.on('data', (data) => {
        console.log(`Stderr: ${data}`);
    });

    ls.on('error', (error) => {
        console.log(`Error: ${error.messages}`);
    });

    ls.on('close', (code) => {
        console.log(`Process exited with code ${code}`);
    });

    // res.json(testStartFluid);
});

router.get('/stop', (req,res) =>{
    
const testStopFluid = {
    msg: "Le button est connecté avec le serveur - STOP"
}

    // Sends JSON, no need to stringify express takes care of everything
    res.json(testStopFluid);
});

export default router;