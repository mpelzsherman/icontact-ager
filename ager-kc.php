<?php

require_once 'config-kc.php';
require_once 'util.php';
require_once 'lib_ager.inc';

date_default_timezone_set('America/New_York');
// workaround for default security settings?
ini_set('allow_url_fopen', 'On');

/***********************************/
/** set your account details here **/
/***********************************/
$accountId = 1245985;
$clientFolderId = 16941;

$prospectList = 23140;
$purchasedList = 23139;
$deadList = 32558;

$customerBroadcastList = 33974;

ageProspects($accountId, $clientFolderId, $prospectList, $purchasedList, $deadList);

agePurchasers($accountId, $clientFolderId, $purchasedList, $customerBroadcastList);

?>
