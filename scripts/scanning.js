// JavaScript for handling the "Scan Bottle" button click
document.getElementById('scan-bottle').addEventListener('click', function () {
    window.location.href = 'scanning.html'; // Make sure the path to scanning.html is correct
});

// scan.js
document.addEventListener('DOMContentLoaded', function () {
    // Simulate scanning process
    setTimeout(function () {
        document.getElementById('scan-bottle').textContent = 'Scanning completed! Please return to redeem points.';

        // Redirect back to dashboard after 3 seconds
        setTimeout(function () {
            window.location.href = 'scanning.html';
            // window.location.href = 'ui.html'; // Replace with actual dashboard page URL
        }, 3000);
    }, 5000);

    // Simulate 5 seconds of scanning
});
