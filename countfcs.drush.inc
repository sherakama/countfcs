<?php

/**
 * @file
 * Adds a drush command to list users.
 */

/**
 * Implements hook_drush_command().
 */
function countfcs_drush_command() {

  $items['countfcs'] = array(
    'description' => 'Counts field collections by field_name.',
    'aliases' => array('cfc'),
    'arguments' => array(
      'field_name' => 'The field name to count on',
    ),
    'examples' => array(
      'drush countfcs field_image_info' => 'Counts field collections on entities for field field_image_info',
    ),
    'core' => array(7),
  );

  return $items;
}

/**
 * Displays a list of users.
 */
function drush_countfcs($field_name = NULL) {

  // Must be a string.
  if (!isset($field_name) || !is_string($field_name)) {
    $available = db_select("field_config", "config")
      ->fields("config", array("field_name"))
      ->condition("type", "field_collection")
      ->execute();

    if ($available->rowCount() == 0) {
      drush_log("Sorry, but there are no field collections available.", "error");
    }

    $options = array();
    while($field = $available->fetchObject()) {
      $options[$field->field_name] = $field->field_name;
    }

    $field_name = drush_choice($options, dt("Which field collection do you want to count?"));
  }

  // Patch things up.
  $fc_name = $field_name; // "field_s_course_section_info";
  $real_name = "field_data_" . $fc_name;
  $total_fcs = 0;

  // Better check this if you don't want flames everywhere.
  if (!db_table_exists($real_name)) {
    drush_log("Field does not exist", "error");
    return;
  }

  // Ok table exists lets see what is in it.
  $fcs = db_query("SELECT entity_id, COUNT(entity_id) as count FROM {$real_name} GROUP BY entity_id");

  // Nothing. :(
  if ($fcs->rowCount() === 0) {
    drush_log("No results found.", "error");
    return;
  }

  // Something!
  while($result = $fcs->fetchAssoc()) {
    $total_fcs += (int) $result["count"];
    drush_log("Entity ID: " . $result['entity_id'] . " has " . $result["count"] . " Field Collection items.", "status");
  }

  drush_log("Total field collections: " . $total_fcs, "status");
  drush_log("Total entities: " . $fcs->rowCount(), "status");

}
