<?php

// read all the parameters and assign them to local variables:
foreach ($_REQUEST as $key => $value)
   {
      if ($key == "zipcode") {
          $zipcode = $value;
      }

      // if ($key == "zipcode2") {
      //     $zipcode2 = $value;
      // }
   }

// if (isset($name) && isset($age) ) {
//     if ($age < 21) {
//         echo "<p> $name, You're not old enough to drink.</p>\n";
//     } else {
//         echo "<p> Hi $name. You're old enough to have a drink, but do ";
//         echo "so responsibly.</p>\n";
//     }
// }
    $url = 
      'http://airnow.gov/index.cfm?action=airnow.local_city&zipcode=10038&submit=Go'; 
    $url2 = 
      'http://airnow.gov/index.cfm?action=airnow.local_city&zipcode=94123&submit=Go';       
    // whether you should check for a value 
    // on the line you're reading:
    $checkForValue = false;
    $checkForValue2 = false;
    // $checkForValue3 = false;

    // value of the Air Quality reading:
     $airQuality = -1;
     $highToday = -1;

    // value of the Air Quality reading:
    $airQuality2 = -1;

    // open the file at the URL for reading:
    $filePath = fopen ($url, "r");

        // open the file at the URL for reading:
    $filePath2 = fopen ($url2, "r");

    // as long as you haven't reached the end of the file:
    while (!feof($filePath))
    {
        // read one line at a time, and strip all HTML and
        // PHP tags from the line:
        $line = fgetss($filePath, 4096);
		// if the current line contains the substring "Current Conditions"
        // then the next line with an integer is the air quality:
        
        if (preg_match('/Current Conditions/', $line)) {
           $checkForValue = true;
        }

        if (preg_match("/Today's High/", $line)) {
           $checkForValue2 = true;
        }

        if ($checkForValue == true && (int)$line > 0){
        	$airQuality = (int)$line;
        	$checkForValue = false;
        }
        if ($checkForValue2 == true && (int)$line > 0){
          $highToday = (int)$line;
          $checkForValue2 = false;
        }

    }



    // as long as you haven't reached the end of the file:
    while (!feof($filePath2))
    {
        // read one line at a time, and strip all HTML and
        // PHP tags from the line:
        $line2 = fgetss($filePath2, 4096);
    // if the current line contains the substring "Current Conditions"
        // then the next line with an integer is the air quality:
        
        if (preg_match('/Current Conditions/', $line2)) {
           $checkForValue2 = true;
        }

        if ($checkForValue2 == true && (int)$line2 > 0){
          $airQuality2 = (int)$line2;
          $checkForValue2 = false;
        }

    }
    
    echo "Air Quality:". $airQuality."\r";
    echo "AirSF:". $airQuality2."\r";

     // echo "Tomorrow's High:". $highTmrw;

    // close the file at the URL, you're done: 
    fclose($filePath);
    fclose($filePath2);

?>

<html>
<body>

<!-- <form action="aqscraper23.php" method="POST" enctype="application/x-www-form-urlencoded">
	Zipcode: <input type="text" name="zipcode" value="" /><br> -->
	<!-- Zipcode 2: <input type="text" name="zipcode2" value="write zipcode here" /> -->
<!-- 	<input type="submit" value="Submit" />
</form> -->

</body>
</html>