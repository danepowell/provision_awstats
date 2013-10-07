<?php

/**
 * Implementation of HOOK_provision_apache_vhost_config
 */

function provision_awstats_provision_apache_vhost_config($uri, $data) {
  $module_dir = dirname(__FILE__);
  $site_dir = d()->site_path;
  $awstats_model = file_get_contents($module_dir . '/awstats.model.conf');
  $awstats_conf_dir = $site_dir . "/files/awstats";
  $awstats_data_dir = $site_dir . "/files/awstats/data";
  $aliases = implode(' ', $data['aliases']);
  $server_config_path = d()->server->config_path;

  // Check that the AWStats folder exists and has the corrent perms
  if (!is_dir($awstats_conf_dir)) {
    provision_file()->mkdir($awstats_conf_dir, 0770, TRUE)
      ->succeed('Created @path and set permissions to @perm')
      ->fail('Could not create @path')
      ->status();
  }

  // Check that the AWStats data folder exists and has the corrent perms
  if (!is_dir($awstats_data_dir)) {
    provision_file()->mkdir($awstats_data_dir, 0770, TRUE)
      ->succeed('Created @path and set permissions to @perm')
      ->fail('Could not create @path')
      ->status();
  }

  // Replace variables in awstats.model.conf
  $search = array(
    'find' => array(
      '{SiteDomain}',
      '{HostAliases}',
      '{DirData}',
    ),
    'replace' => array(
      $uri,
      $aliases,
      $awstats_conf_dir . "/data",
    ),
  );

  $awstats_conf = str_replace($search['find'], $search['replace'], $awstats_model);

  // TODO
  // Check that the AWStats conf folder exists and has the corrent perms
  if (!is_dir($server_config_path . '/awstats')) {
    provision_file()->mkdir($server_config_path . '/awstats', 0770, TRUE)
      ->succeed('Created @path and set permissions to @perm')
      ->fail('Could not create @path')
      ->status();
  }

  // Create SymLink to config file
  provision_file()->unlink($server_config_path . '/awstats/awstats.' . $uri . '.conf', TRUE);
  provision_file()->symlink($awstats_conf_dir . '/awstats.' . $uri . '.conf', $server_config_path . '/awstats/awstats.' . $uri . '.conf', TRUE)
    ->succeed('Created symlink for @path')
    ->fail('Could not create symlink for @path')
    ->status();

  // Write config
  file_put_contents($awstats_conf_dir . '/awstats.' . $uri . '.conf', $awstats_conf);

  $output = array(
  	  'Alias /awstatsclasses "/usr/share/awstats/lib/"',
	  'Alias /awstatsicons/ "/usr/share/awstats/icon/"',
	  'Alias /awstatscss "/usr/share/doc/awstats/examples/css"',
	  'ScriptAlias /awstats/ /usr/lib/cgi-bin/',
	  'Options ExecCGI -MultiViews +SymLinksIfOwnerMatch',
	  );
  return $output;
}
