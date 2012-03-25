<?php

/*
 * for each contact on the "30 Day Prospect" list (listid = 79026)
 *	if contact is not on the "21 Day Post Sale Responder List" (listid  = 79027 ) AND subscription.addDate > 30 days ago:
 *		move contact to "365 Day Broadcast list" (listid = 79028)
 * and
 *
 * for each contact on the "21 Day Post Sale Responder List" (listid  = 79027 )
 *	if subscription.addDate > 21 days ago:
 *		move contact to "365 Day Broadcast list" (listid = 79028)
 * NEW LISTS: 
 * 21 Day Post Sale Bags: 89407 -> 365 BAGS: 89380
 * 21 Day Post Sale Capsules: 89408 -> 365 CAPS: 89379
 */

require_once 'config-okuma.php';
require_once 'util.php';

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

global $prospectAgeLimit, $purchaserAgeLimit;
$prospectAgeLimit = 2592000; // 30 days
$purchaserAgeLimit= 1814400; // 21 days

ageProspects($accountId, $clientFolderId, $prospectList, $purchasedGeneralList, $deadGeneralList);
agePurchasers($accountId, $clientFolderId, $purchasedGeneralList, $deadGeneralList);
agePurchasers($accountId, $clientFolderId, $purchasedBagList, $deadBagList);
agePurchasers($accountId, $clientFolderId, $purchasedCapsList, $deadCapsList);
agePurchasers($accountId, $clientFolderId, $specialOfferVIPs, $threeSixtyFivePioneers);


/****************************************************************************************************
BEYOND HERE BE FUNCTIONS 																						
****************************************************************************************************/

function ageProspects($accountId, $clientFolderId, $prospectList, $purchasedList, $deadList) {
	global $prospectAgeLimit;
	echo "fetching prospects...\n";
	$aProspectResponse = callResource("/a/{$accountId}/c/{$clientFolderId}/subscriptions?listId={$prospectList}&status=normal&limit=10000&offset=0", 'GET');
	$aProspects = $aProspectResponse['data']['subscriptions'];
	
	if (count($aProspects)==0) {
		echo("No contacts on prospect list.\n");
	} else {
		foreach ($aProspects as $aProspect) {
			if (!checkContactStatus($accountId, $clientFolderId, $aProspect)) {
				continue;
			}
			// is prospect already on purchasedList?
			$aPurchasedResponse = callResource("/a/{$accountId}/c/{$clientFolderId}/subscriptions?listId={$purchasedList}&contactId={$aProspect['contactId']}&status=normal", 'GET');
			$aSubs = $aPurchasedResponse['data']['subscriptions'];
			//print_r($aSubs);
			$bProspectPurchased = (count($aSubs) > 0);
			$bTimeElapsed = (time() - strtotime($aProspect['addDate']) > $prospectAgeLimit);
			if ( !$bProspectPurchased && $bTimeElapsed ) {
				// move to deadList
				moveToDeadList($accountId, $clientFolderId, $aProspect, $deadList);
			} else {
				echo "bProspectPurchased: {$bProspectPurchased}; bTimeElapsed: {$bTimeElapsed}\n";
				echo "NOT moving contactId {$aProspect['contactId']} to dead list\n";		
			}
		}
	}
}

function agePurchasers($accountId, $clientFolderId, $purchasedList, $deadList) {
	global $purchaserAgeLimit;
	echo "fetching purchasers...\n";
	$aPurchaserResponse = callResource("/a/{$accountId}/c/{$clientFolderId}/subscriptions?listId={$purchasedList}&status=normal&limit=10000&offset=0", 'GET');
	$aPurchasers= $aPurchaserResponse['data']['subscriptions'];
	
	if (count($aPurchasers)==0) {
		die("No contacts on purchaser list $purchasedList.\n");
	}
	
	foreach ($aPurchasers as $aPurchaser) {
		if (!checkContactStatus($accountId, $clientFolderId, $aPurchaser)) {
			continue;
		}
		$iElapsedTime = time() - strtotime($aPurchaser['addDate']);
		$bTimeElapsed = ($iElapsedTime > $purchaserAgeLimit);
		if ($bTimeElapsed) {
			// move to deadList
			moveToDeadList($accountId, $clientFolderId, $aPurchaser, $deadList);
		} else {
			echo "NOT moving contactId {$aPurchaser['contactId']} to dead list $deadList because elapsed time ({$iElapsedTime}) < {$purchaserAgeLimit}\n";
		}
	}
	
	echo "done!\n";
}

function moveToDeadList($accountId, $clientFolderId, $aProspect, $deadList) {
	echo "moving contact to dead list:\n" . var_export($aProspect, true) . "\n";
	$sData = "<subscription><listId>{$deadList}</listId></subscription>";
//	$fstream = 'data://text/plain;base64,'.base64_encode($sData);	
	callResource("/a/{$accountId}/c/{$clientFolderId}/subscriptions/{$aProspect['subscriptionId']}", 'PUT', $sData, 'xml', false);
}


function checkContactStatus($accountId, $clientFolderId, $aProspect) {
	$iContactId = $aProspect['contactId'];
	$aResponse = callResource("/a/{$accountId}/c/{$clientFolderId}/contacts/{$iContactId}", 'GET');
	//var_export($aResponse);
	$sStatus = $aResponse['data']['contact']['status'];
	$bStatusIsNormal = ($sStatus == "normal");
	if (!$bStatusIsNormal) {
		echo "contactId {$aProspect['contactId']} status ({$sStatus}) is not normal... skipping\n";
	}
	return $bStatusIsNormal;
}
