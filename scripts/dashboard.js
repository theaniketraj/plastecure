// 1️⃣ Firebase config (same as login.js)
const firebaseConfig = {
    apiKey: "AIzaSyCy8IqIztOOIOEDZyjrBQLOkz8DUMsj0ls",
    authDomain: "plastecure.firebaseapp.com",
    databaseURL: "https://plastecure-default-rtdb.asia-southeast1.firebasedatabase.app",
    projectId: "plastecure",
    storageBucket: "plastecure.firebasestorage.app",
    messagingSenderId: "258042144487",
    appId: "1:258042144487:web:751a5b4c3fec9739cfa442",
}

// 2️⃣ Initialize Firebase
firebase.initializeApp(firebaseConfig);
const database = firebase.database();

// 3️⃣ Load user info from localStorage
const userName = localStorage.getItem("userName") || "Guest";
const userId = localStorage.getItem("userId");
if (!userId) {
    alert("No user logged in. Redirecting to Login.");
    window.location.href = "login.html";
    throw new Error("Missing userId");
}

// 4️⃣ Populate user‐info popup
document.getElementById("userName").textContent = userName;
document.getElementById("userId").textContent = userId;

// 5️⃣ Fetch Bottle Count & Rewards
database
    .ref("users/" + userId)
    .once("value")
    .then(snapshot => {
        if (snapshot.exists()) {
            const data = snapshot.val();
            document.getElementById("bottle-count").textContent = data.bottle_count ?? 0;
            document.getElementById("total-rewards").textContent = data.total_rewards ?? 0;
        } else {
            throw new Error("User data not found");
        }
    })
    .catch(err => {
        console.error("Error loading user data:", err);
        alert("Could not load your data. Please try again.");
    });

// 6️⃣ Existing UI logic
// Hover popup
document.getElementById('user-icon').addEventListener('mouseenter', () => {
    document.querySelector('.user-info-popup').style.display = 'block';
});
document.getElementById('user-icon').addEventListener('mouseleave', () => {
    document.querySelector('.user-info-popup').style.display = 'none';
});
