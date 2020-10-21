import express from 'express';
import path from 'path';
import {fileURLToPath} from 'url';

const __filename = fileURLToPath(import.meta.url);
const __dirname = path.dirname(__filename);

const router = express.Router();



const testStopFluid = {
    msg: "Le button est connecté avec le serveur - STOP"
}


router.post('/start', (req,res) =>{
    const _passPhrase = req.body.passphrase;

    console.log(_passPhrase);

    const testStartFluid = {
        msg: "Le button est connecté avec le serveur - START",
        passphrase: _passPhrase
    }

    
    res.json(testStartFluid);
});

router.get('/stop', (req,res) =>{
    // Sends JSON, no need to stringify express takes care of everything
    res.json(testStopFluid);
});

export default router;