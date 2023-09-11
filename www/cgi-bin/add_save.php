<?php

// Parse the query string
parse_str($requestData);

// Get data from the parsed request
$a = isset($requestData['a']) ? $requestData['a'] : null;
$b = isset($requestData['b']) ? $requestData['b'] : null;

function ft_add($a, $b) {
    return $a + $b;
}

if ($a !== null && $b !== null) {
    $a = intval($a);
    $b = intval($b);

    $sum = ft_add($a, $b);

    echo "<h1>Result :</h1>";
    echo "<p>$a + $b = $sum.</p>";

} else {
    echo "Content-type: text/html\r\n";
    echo "\r\n";
    echo "<h1>Error :</h1>";
    echo "<p>'a' and 'b' parameters required.</p>";
}
?>

