<?php

// Check if the request method is POST
if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    $a = $_POST['a'];
    $b = $_POST['b'];
} else {
    // Check if the request method is GET
    $a = $_GET['a'];
    $b = $_GET['b'];
}

// Add the numbers
$result = $a + $b;

// Output the result
echo "Content-type: text/html\r\n";
echo "\r\n";
echo "<h1>Result :</h1>";
echo "<p>$a + $b = $result.</p>";