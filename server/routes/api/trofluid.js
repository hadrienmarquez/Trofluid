import express from 'express';
import path from 'path';
import {fileURLToPath} from 'url';
import {spawn} from 'child_process';

const __filename = fileURLToPath(import.meta.url);
const __dirname = path.dirname(__filename);

const router = express.Router();


router.post('/start', (req,res) =>{

    const _passPhrase = req.body.passphrase;
    const _source = req.body.source;
    
    let _scriptCMD = '/usr/local/scripts/startStream.sh';

    if(req.body.skip_passphrase)
    {
        _scriptCMD += ` -s ${_source}`;
    } else {
        _scriptCMD += ` -s ${_source} -p ${_passPhrase}`
    }

    console.log(_scriptCMD);

    // Open ssh with communication with ffmpeg server and launch stream
    const cmd = spawn(`ssh ffmpeg@trofluid_ffmpeg_1 "bash -c '${_scriptCMD}'"`, {shell: true});

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