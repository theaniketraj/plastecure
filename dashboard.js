// dashboard.js
document.getElementById('user-icon').addEventListener('mouseenter', function() {
    // Show popup when hovering over the user icon
    document.querySelector('.user-info-popup').style.display = 'block';
});

document.getElementById('user-icon').addEventListener('mouseleave', function() {
    // Hide popup when not hovering over the user icon
    document.querySelector('.user-info-popup').style.display = 'none';
});

<script>
    {/* {/* // Example: Replace these values with actual user data from an API, local storage, or form input */}
    const userName = "Jane Smith";  // Replace with dynamic user name
    const userId = "98765";         // Replace with dynamic user ID

    // Set the values to the appropriate elements
    document.getElementById("userName").textContent = userName;
    document.getElementById("userId").textContent = userId;
</script>

// Get the user data from localStorage
const userName = localStorage.getItem("userName");
const userId = localStorage.getItem("userId");

// Set the retrieved values in the popup elements
document.getElementById("userName").textContent = userName || "Jane Doe";  // Fallback to 'Jane Doe'
document.getElementById("userId").textContent = userId || "98765";         // Fallback to '98765'


// dashboard.js
document.addEventListener('DOMContentLoaded', function() {
    const scanButton = document.getElementById('scanBottleBtn');
    
    // Check if the button exists
    if (scanButton) {
        // Add an event listener to the button
        scanButton.addEventListener('click', function() {
            window.location.href = 'scanning.html'; // Adjust the path if needed
        });
    } else {
        console.error('Scan Bottle button not found!');
    }

    scanButton.addEventListener('click', function() {
        alert('Scan Bottle button clicked!'); // For debugging
        window.location.href = 'scanning.html';
    });
    
});
