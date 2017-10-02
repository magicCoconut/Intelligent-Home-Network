   <!-- SCE205 Assessment 1 
   @author Yudi An
   1201562
   Y3-ICS
   PHP Website Server -->
 

<?php
// prevent the server from timing out
set_time_limit(0);

// include the web sockets server script (the server is started at the far bottom of this file)
// USING "class.PHPWebSocket.php" AS REFERENCE.
require 'class.PHPWebSocket.php';

// when a client sends data to the server
function Message($clientID, $message, $messageLength, $binary) {
	global $Server;
	//for each vistor send what "I" said
	foreach ( $Server->wsClients as $id => $client )
		date_default_timezone_set("Asia/Hong_Kong"); 
		$nowtime = time();
		//excapt "me"
		if ( $id != $clientID )
			$Server->wsSend($id, "c@Visitor $clientID|time()| said \"$message\"");
}

// when a client connects
function Open($clientID)
{

	global $Server;
	$sum = sizeof($Server->wsClients)-1;//excepte "me" how many 

	//told the user how many people here.
	if ( sizeof($Server->wsClients) == 1 )		
		$Server->wsSend($clientID, "s@Only you in this chat room.");
	else if(sizeof($Server->wsClients) ==2) 		
		$Server->wsSend($clientID,"s@There is 1 clients in this chat room. You can talk to him");
	else 
		$Server->wsSend($clientID,"s@There is \"$sum\" clients in this chat room. You can talk to him");

	//Send a join notice to everyone but the person who joined
	foreach ( $Server->wsClients as $id => $client )
		if ( $id != $clientID )
			$Server->wsSend($id, "s@Visitor $clientID has joined the room.");
}

// when a client closes or lost connection
function Close($clientID, $status) {
	global $Server;
	//Send a user left notice to everyone in the room
	foreach ( $Server->wsClients as $id => $client )
		$Server->wsSend($id, "s@Visitor $clientID has left the room.");
}

// start the server
$Server = new PHPWebSocket();
$Server->bind('message', 'Message');
$Server->bind('open', 'Open');
$Server->bind('close', 'Close');
// for other computers to connect, you will probably need to change this to your LAN IP or external IP,
// alternatively use: gethostbyaddr(gethostbyname($_SERVER['SERVER_NAME']))
$Server->wsStartServer('192.168.1.33', 80);


?>