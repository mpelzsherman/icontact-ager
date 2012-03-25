<?php

require_once 'config-zen.php';
require_once 'util.php';
require_once 'lib_ager.inc';

date_default_timezone_set('America/New_York');
// workaround for default security settings?
ini_set('allow_url_fopen', 'On');

/***********************************/
/** set your account details here **/
/***********************************/
$accountId = 1035649;
$clientFolderId = 7846;

$purchasedList = 11911; // Zen Central New Members
$deadList = 16313; // Zen Central 365 Day Broadcast List
$purchaserAgeLimit = 2851200; // 33 days

agePurchasers($accountId, $clientFolderId, $purchasedList, $deadList, $purchaserAgeLimit);

?>