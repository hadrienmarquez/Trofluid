// Opens the popup window
function openPopup(){
    document.querySelector('.popup-container').style.display = "flex";
};

// Close the popup window
function closePopup(){
    document.querySelector('.popup-container').style.display = "none";
};


function checkInPassphrase(){

    const onlyAlphabetChar = /^[a-z]+$/gi

    // Length is valid?
    if(document.getElementById('popup').elements['passphrase-input'].value.length < 10){

        // Prompt something and go back
        alert("Passphrase not valid! Passphrase must be at least ten characters.");
        document.getElementById('popup').elements['passphrase-input'].value = "";
        return false;

    } else if (!(onlyAlphabetChar.test(document.getElementById('popup').elements['passphrase-input'].value))) {

        alert("Passphrase not valid! Only aphabetic characters are accepted.");
        document.getElementById('popup').elements['passphrase-input'].value = "";
        return false;
    } else {
        // Passphrase is good
        return true;
    }


};

// Funtion added to the event listener binded to the passphrase form
function submitPassphrase(e){
    // Prevent default submit action
    e.preventDefault();

    if(checkInPassphrase()){
        let source;
        if(document.getElementById('decklink').checked) {
            source = 'decklink';
         } else if(document.getElementById('mire').checked) {
            source = 'Bars_legal.0000000.jpg';
        }

        // Populating JSON with form data
        let formValue = {
            passphrase: document.getElementById('popup').elements['passphrase-input'].value,
            source: source, 
            skip_passphrase: false
        };

        // Cleaning the input value
        document.getElementById('popup').elements['passphrase-input'].value = "";
        
        // create request object
        const request = new Request('/api/trofluid/start', {
            method: 'POST',
            headers: new Headers({
                'Content-Type': 'application/json'
            }),
            body: JSON.stringify(formValue)
        });

        // Send request
        fetch(request)
        .then((res) => res.json)
        .then((res) => {
            // WHAT TO DO WITH RESPONSE
            console.log(res);
        }).catch((err) => {
            console.log(err);
        });

        // Removing popup
        closePopup();

    }

};


function skipPassphrase() {
    let source;
    if(document.getElementById('decklink').checked) {
        source = 'decklink';
     } else if(document.getElementById('mire').checked) {
        source = 'Bars_legal.0000000.jpg';
    }

    // Populating JSON with form data
    let formValue = {
        passphrase: 'none',
        source: source,    
        skip_passphrase: true
    };

    // Cleaning the input value
    document.getElementById('popup').elements['passphrase-input'].value = "";
    
    // create request object
    const request = new Request('/api/trofluid/start', {
        method: 'POST',
        headers: new Headers({
            'Content-Type': 'application/json'
        }),
        body: JSON.stringify(formValue)
    });

    // Send request
    fetch(request)
    .then((res) => res.json)
    .then((res) => {
        // WHAT TO DO WITH RESPONSE
        console.log(res);
    }).catch((err) => {
        console.log(err);
    });

    // Removing popup
    closePopup();

}


// Binding the event listener to the form and the submitPassphrase function
document.getElementById('popup').addEventListener('submit', submitPassphrase);

