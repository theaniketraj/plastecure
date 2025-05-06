// login.js
document.getElementById('next-btn').addEventListener('click', function () {
    // Assuming user logs in and you store the name and ID
    localStorage.setItem("userName", "userName");
    localStorage.setItem("userId", "userId");

    const name = document.getElementById('name').value;
    const uniqueId = document.getElementById('unique-id').value;

    if (name && uniqueId) {
        // Process the login information or store it
        // Then, redirect to the next page (replace with your actual page)
        window.location.href = 'dashboard.html';
    } else {
        alert('Please enter both your name and unique ID.');
    }

    const db = firebase.firestore();

    // Get a specific document (user) by ID
    const userId = "someUserId";  // Replace this with the actual user ID or get it dynamically

    db.collection("users").doc(userId).get().then((doc) => {
        if (doc.exists) {
            console.log("User Data:", doc.data());
            document.getElementById('userName').innerText = doc.data().name;
            document.getElementById('userId').innerText = doc.data().userId;
        } else {
            console.log("No such document!");
        }
    }).catch((error) => {
        console.log("Error getting document:", error);
    });

});
