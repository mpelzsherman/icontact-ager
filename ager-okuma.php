<?php

require_once 'config-okuma.php';
require_once 'util.php';
require_once 'lib_ager.inc';

date_default_timezone_set('America/New_York');
// workaround for default security settings?
ini_set('allow_url_fopen', 'On');

/***********************************/
/** set your account details here **/
/***********************************/
$accountId = 510324;
$clientFolderId = 15622;

$prospectList = 79026; // "30-day prospect list"

$purchasedGeneralList = 79027; // "21 Day Post Sale Responder"
$deadGeneralList = 79028;  // "365-day Broadcast List"

$purchasedBagList = 89407;
$deadBagList = 89380;

$purchasedCapsList = 89408;
$deadCapsList = 89379;

$specialOfferVIPs = 89090;
$threeSixtyFivePioneers = 89359;

$prospectAgeLimit = 2592000; // 30 days
$purchaserAgeLimit= 1814400; // 21 days

// Defunct Lists
//ageProspects($accountId, $clientFolderId, $prospectList, $purchasedGeneralList, $deadGeneralList, $prospectAgeLimit);
//agePurchasers($accountId, $clientFolderId, $purchasedGeneralList, $deadGeneralList, $purchaserAgeLimit);
//agePurchasers($accountId, $clientFolderId, $specialOfferVIPs, $threeSixtyFivePioneers, $purchaserAgeLimit);

agePurchasers($accountId, $clientFolderId, $purchasedBagList, $deadBagList, $purchaserAgeLimit);
agePurchasers($accountId, $clientFolderId, $purchasedCapsList, $deadCapsList, $purchaserAgeLimit);

?>
