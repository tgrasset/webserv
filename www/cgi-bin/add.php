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

echo "<!DOCTYPE html>

<html lang=\"en\">\n
    <head>\n
    <meta charset=\"UTF-8\">\n
    <meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\">\n
    <title>Basic Calculator</title>\n
</head>\n
<body>";
echo "<h1>Result :</h1>";
echo "<p>$a + $b = $result</p>";
echo "</body>\n
</html>\n";
?>