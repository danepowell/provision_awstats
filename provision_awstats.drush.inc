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
      '{LogFile}',
    ),
    'replace' => array(
      $uri,
      $aliases,
      $awstats_conf_dir . "/data",
      '/var/log/apache2/' . $uri . '.access.log',
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

  // Write config
  file_put_contents($server_config_path . '/awstats/awstats.' . $uri . '.conf', $awstats_conf);
}
