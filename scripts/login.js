// 1️⃣ Firebase config
const firebaseConfig = {
    apiKey: "AIzaSyCy8IqIztOOIOEDZyjrBQLOkz8DUMsj0ls",
    authDomain: "plastecure.firebaseapp.com",
    databaseURL: "https://plastecure-default-rtdb.asia-southeast1.firebasedatabase.app",
    projectId: "plastecure",
    storageBucket: "plastecure.firebasestorage.app",
    messagingSenderId: "258042144487",
    appId: "1:258042144487:web:751a5b4c3fec9739cfa442"
};

// 2️⃣ Initialize Firebase
firebase.initializeApp(firebaseConfig);
const database = firebase.database();

// 3️⃣ On click, validate user ID exists
document.getElementById("next-btn").addEventListener("click", () => {
    const username = document.getElementById("username").value.trim();
    const userId = document.getElementById("userId").value.trim();

    if (!username || !userId) {
        alert("Please enter both name and unique ID");
        return;
    }

    const userRef = database.ref("users/" + userId);
    userRef.once("value")
        .then(snapshot => {
            if (snapshot.exists()) {
                // Store and proceed
                localStorage.setItem("userName", username);
                localStorage.setItem("userId", userId);
                window.location.href = "dashboard.html";
            } else {
                alert("User ID not found!");
            }
        })
        .catch(error => {
            console.error("Error checking user ID:", error);
            alert("An error occurred. Please try again later.");
        });
});
