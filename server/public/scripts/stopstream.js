function stopStream(){
    fetch('/api/trofluid/stop')
    .then((res) => res.json)
    .then((res) => {
        console.log(res);
    }).catch((err) => {
        console.log(err);
    });
};