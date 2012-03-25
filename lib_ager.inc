<?php
/*
 * for each contact on the $prospectList
 *	if contact is not on the $purchasedList AND now - subscription.addDate > $prospectAgeLimit
 *	    move contact to $deadList
 */
function ageProspects($accountId, $clientFolderId, $prospectList, $purchasedList, $deadList, $prospectAgeLimit) {
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

/*
 * for each contact on the $purchasedList
 *	if now - subscription.addDate > $purchaserAgeLimit
 *		move contact to $deadList
 */
function agePurchasers($accountId, $clientFolderId, $purchasedList, $deadList, $purchaserAgeLimit) {
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

?>