import express from 'express';
import path from 'path';
import {fileURLToPath} from 'url';
import router from './routes/api/trofluid.js'

const __filename = fileURLToPath(import.meta.url);
const __dirname = path.dirname(__filename);

const server = express();

const PORT =  process.env.PORT || 8080;

server.use(express.static(path.join(__dirname, 'public',)));

server.use(express.json());
server.use(express.urlencoded({extended: false}));

server.use('/api/trofluid', router);


server.listen(PORT, () => console.log(`Server started on port ${PORT}`));