<?php

require_once 'config-okuma.php';
require_once 'util.php';

$accountId = 510324;
$clientFolderId = 15622;

$listId = $argv[1];

$result = callResource("/a/{$accountId}/c/{$clientFolderId}/lists/$listId", 'GET');

var_export($result);