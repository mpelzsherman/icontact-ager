<?php
/*
 * for each contact on the $prospectList
 *	if contact is not on the $purchasedList AND now - subscription.addDate > $prospectAgeLimit
 *	    move contact to $deadList
 */
function ageProspects($accountId, $clientFolderId, $prospectList, $purchasedList, $deadList, $prospectAgeLimit=30, $bDryRun=false) {
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
            $aContact = callResource("/a/{$accountId}/c/{$clientFolderId}/contacts/{$aProspect['contactId']}", 'GET');
			// is prospect already on purchasedList?
			$aPurchasedResponse = callResource("/a/{$accountId}/c/{$clientFolderId}/subscriptions?listId={$purchasedList}&contactId={$aProspect['contactId']}&status=normal", 'GET');
			$aSubs = $aPurchasedResponse['data']['subscriptions'];
			//print_r($aSubs);
			$bProspectPurchased = (count($aSubs) > 0);
            $iElapsedDays = floor((time() - strtotime($aProspect['addDate'])) / (24 * 60 * 60));
			$bTimeElapsed = ($iElapsedDays > $prospectAgeLimit);
			if ( !$bProspectPurchased && $bTimeElapsed ) {
                echo "{$aContact['data']['contact']['email']} has not purchased for $iElapsedDays days. ";
				// move to deadList
				moveToDeadList($accountId, $clientFolderId, $aProspect, $deadList, $bDryRun);
			} else {
				echo "NOT moving {$aContact['data']['contact']['email']} to dead list. ";
                echo "bProspectPurchased: $bProspectPurchased; added $iElapsedDays days ago ($bTimeElapsed)\n";
			}
		}
	}
}

/*
 * for each contact on the $purchasedList
 *	if now - subscription.addDate > $purchaserAgeLimit
 *		move contact to $deadList
 */
function agePurchasers($accountId, $clientFolderId, $purchasedList, $deadList, $purchaserAgeLimit=30) {
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
        $aContact = callResource("/a/{$accountId}/c/{$clientFolderId}/contacts/{$aPurchaser['contactId']}", 'GET');
        $iElapsedDays = floor((time() - strtotime($aPurchaser['addDate'])) / (24 * 60 * 60));
		$bTimeElapsed = ($iElapsedDays > $purchaserAgeLimit);
		if ($bTimeElapsed) {
            echo "{$aContact['data']['contact']['email']} has not purchased for $iElapsedDays days. ";
			// move to deadList
			moveToDeadList($accountId, $clientFolderId, $aPurchaser, $deadList);
		} else {
            $aContact = callResource("/a/{$accountId}/c/{$clientFolderId}/contacts/{$aPurchaser['contactId']}", 'GET');
			echo "NOT moving {$aContact['data']['contact']['email']} to dead list $deadList because elapsed time ({$iElapsedDays}) < {$purchaserAgeLimit}\n";
		}
	}

	echo "done!\n";
}

function moveToDeadList($accountId, $clientFolderId, $aProspect, $deadList, $bDryRun=false) {
    $aContact = callResource("/a/{$accountId}/c/{$clientFolderId}/contacts/{$aProspect['contactId']}", 'GET');
	echo "moving {$aContact['data']['contact']['email']} to dead list\n";
    if ($bDryRun) {
        echo "dry run - not really moving\n";
        return;
    }
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