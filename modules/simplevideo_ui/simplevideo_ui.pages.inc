<?php
// $Id$

/**
 * @file
 * User interface form administer simplevideo.
 */

/**
 * List simplevideo preset
 */
function simplevideo_ui_preset_overview() {
  $header = array(t('Preset Name'), t('Actions'));
  $result = db_query("SELECT presetname, presetid FROM {simplevideo_preset}");
  $rows = array();
  while ($presets = db_fetch_array($result)) {
    $row = array();
    $row[] = l($presets['presetname'], 'admin/build/simplevideo/'. $presets['presetid']);
    $links = array();
    $links[] = l(t('Edit'), 'admin/build/simplevideo/'. $presets['presetid']);
    $links[] = l(t('Delete'), 'admin/build/simplevideo/'. $presets['presetid'] .'/delete');
    $row[] = implode('&nbsp;&nbsp;&nbsp;&nbsp;', $links);
    $rows[] = $row;
  }
  $output = theme('table', $header, $rows);
  return $output;
}


/**
 * Preset Admin callbacks and required functions.
 */
function simplevideo_ui_preset_form(&$form_state, $arg = NULL) {
  $presetid = $arg;
  $form = array();

  $form['presetid'] = array(
      '#type' => 'value',
      '#value' => $presetid,
  );

  $form['presetname'] = array(
      '#type' => 'textfield',
      '#size' => '64',
      '#title' => t('Preset Namespace'),
      '#description' => t('The namespace is used in URL\'s for images to tell simplevideo how to process an video. Please only use alphanumeric characters'),
      '#element_validate' => array('simplevideo_ui_presetname_validate'),
  );
  $form['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Save Preset'),
      '#weight' => 9,
  );
  return $form;
}

function simplevideo_ui_preset_actions_form($form_state, $arg) {
  $presetid = $arg;
  $form = array();

  $form['presetid'] = array(
      '#type' => 'value',
      '#value' => $presetid,
  );

  $form['presetname'] = array(
      '#type' => 'textfield',
      '#title' => t('Preset Namespace: '),
      '#default_value' => simplevideo_get_presetname($presetid),
      //'#element_validate' => array('simplevideo_ui_presetname_validate'),
      '#size' => 32,
  );
  $form['update_preset'] = array(
      '#type' => 'submit',
      '#value' => 'Update preset',
      '#weight' => '100',
  );
  $form['run'] = array(
      '#type' => 'radios',
      '#title' => t('How to Apply a preset'),
      '#options' => array(t('By node save/node update'), t('By cron')),
      '#default_value' => simplevideo_get_scheduled($presetid),
      '#element_validate' => array('simplevideo_ui_preset_run_validate'),
  );
  $form['preset_settings'] = array(
      '#type' => 'fieldset',
      '#title' => t('Actions:'),
      '#tree' => TRUE,
      '#theme' => 'simplevideo_ui_preset_actions',
  );

  $form['preset_settings']['presetid'] = array(
      '#type' => 'value',
      '#value' => $presetid,
  );

  $form['preset_settings']['actions'] = array(
      '#type' => 'fieldset',
      '#title' => t('New action '),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
  );

  $form['preset_settings']['actions']['convert'] = array(
      '#type' => 'item',
      '#value' => l(t('Encoding video'), 'admin/build/simplevideo/'. $presetid .'/add/encoding'),
  );

  $form['preset_settings']['actions']['thumbnails'] = array(
      '#type' => 'item',
      '#value' => l(t('Create thumbnails'), 'admin/build/simplevideo/'. $presetid .'/add/thumbnails'),
  );
  return $form;
}

function simplevideo_ui_preset_run_validate($form, &$form_state) {
  if (!module_exists('simplevideo_cron')) {
    form_set_error('simplevideo_ui_preset_actions_form', t('Module simplevideoCron is not included'));
  }
}

function simplevideo_ui_preset_actions_form_submit($form, &$form_state) {

  $presetid = $form_state['values']['presetid'];
  $presetname = $form_state['values']['presetname'];
  $run = $form_state['values']['run'];
  simplevideo_preset_update($presetid, $run, $presetname);
   $form_state['redirect'] = 'admin/build/simplevideo';
}

function simplevideo_ui_presetname_validate($form, &$form_state) {
  $values = $form_state['values'];
  // Check for duplicates
  foreach (simplevideo_presets() as $preset) {
    if ($values['presetname'] == $preset) {
      form_set_error('presetname', t('The preset name you have chosen is already in use.'));
      break;
    }
  }

  // Check for illegal characters in preset names
  if (preg_match('/[^0-9a-zA-Z_\-]/', $values['presetname'])) {
    form_set_error('presetname', t('Please only use alphanumeric characters, underscores (_), and hyphens (-) for preset names.'));
  }
}

function simplevideo_ui_preset_form_submit($form, &$form_state) {
  $preset =  $form_state['values']['presetname'];
  $form_state['redirect'] = 'admin/build/simplevideo';
  simplevideo_preset_save($preset);
}

function simplevideo_ui_preset_delete_form($form_state, $arg = NULL) {
  $presetid = $arg;
  $presetname = simplevideo_get_presetname($presetid);
  $question = t("Are you sure you want to delete the preset !presetname", array('!presetname' => $presetname));
  $form['presetid'] = array(
    '#type' => 'value',
    '#value' => $presetid,
  );
  return confirm_form($form, $question, 'admin/build/simplevideo',
  t('This action cannot be undone.'), t('Delete'),  t('Cancel'));
}

function simplevideo_ui_preset_delete_form_submit($form, &$form_state) {
  $presetid = $form_state['values']['presetid'];
  $form_state['redirect'] = 'admin/build/simplevideo';
  simplevideo_delete_preset($presetid);
}

function simplevideo_ui_action_enc_delete_form($form_state, $presetid = NULL, $actionid = NULL) {
  $question = t("Are you sure you want to delete this action");
  $form['presetid'] = array(
      '#type' => 'value',
      '#value' => $presetid,
  );
  $form['actionid'] = array(
      '#type' => 'value',
      '#value' => $actionid,
  );
  return confirm_form($form, $question, 'admin/build/simplevideo',
  t('This action cannot be undone.'), t('Delete'),  t('Cancel'));
}

function simplevideo_ui_action_enc_delete_form_submit($form, &$form_state) {
  $actionid = $form_state['values']['actionid'];
  $presetid = $form_state['values']['presetid'];
  $form_state['redirect'] = 'admin/build/simplevideo/'. $presetid;
  simplevideo_delete_action_enc($actionid);
}
function simplevideo_ui_action_thumb_delete_form($form_state, $presetid = NULL, $actionid = NULL) {
  $question = t("Are you sure you want to delete this action");
  $form['presetid'] = array(
      '#type' => 'value',
      '#value' => $presetid,
  );
  $form['actionid'] = array(
      '#type' => 'value',
      '#value' => $actionid,
  );
  return confirm_form($form, $question, 'admin/build/simplevideo',
  t('This action cannot be undone.'), t('Delete'),  t('Cancel'));
}

function simplevideo_ui_action_thumb_delete_form_submit($form, &$form_state) {
  $actionid = $form_state['values']['actionid'];
  $presetid = $form_state['values']['presetid'];
  $form_state['redirect'] = 'admin/build/simplevideo/'. $presetid;
  simplevideo_delete_action_thumb($actionid);
}

function simplevideo_ui_encoding_form($form_state, $presetid = NULL, $actionid = NULL) {
  $settings = simplevideo_get_enc_settings($actionid);
  $form = array();

  $form['presetid'] = array(
      '#type' => 'value',
      '#value' => $presetid,
  );

  $form['actionid'] = array(
      '#type' => 'value',
      '#value' => $actionid,
  );
  
  $form['video'] = array(
  '#type' => 'fieldset',
  '#title' => t('Video settings'),
  '#collapsible' => TRUE,
  '#collapsed' => FALSE,
  );

  $form['video']['width'] = array(
      '#type' => 'textfield',
      '#title' => t('Width'),
      '#description' => t('Width video. By default 320px.'),
      '#size' => 20,
      '#default_value' => $settings['enc']['width'] ? $settings['enc']['width'] : '320',
      '#required' => TRUE
  );

  $form['video']['height'] = array(
      '#type' => 'textfield',
      '#title' => t('Height'),
      '#description' => t('Height video. By default 240px.'),
      '#size' => 20,
      '#default_value' => $settings['enc']['height'] ? $settings['enc']['height'] : '240',
      '#required' => TRUE
  );

  $form['video']['bitrate'] = array(
      '#type' => 'textfield',
      '#title' => t('Bitrate'),
      '#description' => t('Bitrate video in kbits/sec. By default  500 kbits/sec'),
      '#size' => 20,
      '#default_value' => $settings['enc']['bitrate'] ? $settings['enc']['bitrate'] : 500,
      '#required' => TRUE
  );
  $form['video']['fps'] = array(
      '#type' => 'textfield',
      '#title' => t('FPS'),
      '#description' => t('Framerate. By default 25 frame per second'),
      '#size' => 20,
      '#default_value' => $settings['enc']['fps'] ? $settings['enc']['fps'] : '25',
      '#required' => TRUE,
  );
  $form['video']['start_time'] = array(
      '#type' => 'textfield',
      '#title' => t('Start time'),
      '#description' => t('The time from which to start encoding video. If not specified, then first.'),
      '#size' => 20,
      '#default_value' => $settings['enc']['start_time'],
      '#required' => FALSE,
      '#element_validate' => array('simplevideo_ui_time_validate'),
  );
  $form['video']['duration'] = array(
      '#type' => 'textfield',
      '#title' => t('Duration'),
      '#description' => t('Duration of video encoding. If not specified, then full duration video'),
      '#size' => 20,
      '#default_value' => $settings['enc']['duration'],
      '#required' => FALSE,
      '#element_validate' => array('simplevideo_ui_time_validate'),
  );
  $form['video']['format'] = array(
      '#type' => 'radios',
      '#title' => t('Format video'),
      '#description' => t('Select format video. Must be installed the necessary codecs'),
      '#default_value' => $settings['enc']['format'],
      '#options' => array(t('FLV'), t('H.264')),
  );

  $form['audio'] = array(
  '#type' => 'fieldset',
  '#title' => t('Audio settings'),
  '#collapsible' => TRUE,
  '#collapsed' => FALSE,
  );

  $form['audio']['a_bitrate'] = array(
    '#type' => 'textfield',
    '#title' => t('Audio bitrate in kbit/sec'),
    '#size' => 20,
    '#default_value' => $settings['enc']['a_bitrate'],
    '#required' => TRUE
  );

  $form['audio']['sampling'] = array(
    '#type' => 'select',
    '#title' => t('Audio sampling rate'),
    '#required' => TRUE,
    '#default_value' => $settings['enc']['sampling'],
    '#options' => array(
      '8000' => t('8000 Hz'),
      '11025' => t('11025 Hz'),
      '22050' => t('22050 Hz'),
      '44100' => t('44100 Hz'),
      '48000' => t('48000 Hz'),
    ),
  );
  
  $form['save'] = array(
      '#type' => 'submit',
      '#value' => t('Save action'),
  );

  $form["#validate"][] = "simplevideo_ui_alpha_numeric_validate";

  return $form;
}
function simplevideo_ui_encoding_form_validate($form, &$form_state) {
  $presetid = $form_state['values']['presetid'];
  $actionid = $form_state['values']['actionid'];
  // if Save
  if (empty($actionid)) {
    $result = db_query("SELECT actionid FROM {simplevideo_enc_action} WHERE presetid =%d", $presetid);
    while ($row = db_fetch_array($result)) {
      $actionid_enc[] = $row['actionid'];
    }
    $result = db_query("SELECT actionid FROM {simplevideo_thumb_action} WHERE presetid =%d", $presetid);
    while ($row = db_fetch_array($result)) {
      $actionid_thumb[] = $row['actionid'];
    }
    //Not 2 actions in the preset
    if (count($actionid_enc) >= 1 || count($actionid_thumb) >= 1 ) {
      form_set_error('simplevideo_ui_encoding_form', t('In the preset already have a action, please create a new preset'));
    }
  }
}

function simplevideo_ui_encoding_form_submit($form, &$form_state) {
  $presetid = $form_state['values']['presetid'];
  $actionid = $form_state['values']['actionid'];
  $form_state['redirect'] = 'admin/build/simplevideo/'. $presetid;
  $action = array();
  $action['width'] = $form_state['values']['width'];
  $action['height'] = $form_state['values']['height'];
  $action['bitrate'] = $form_state['values']['bitrate'];
  $action['a_bitrate'] = $form_state['values']['a_bitrate'];
  $action['sampling'] = $form_state['values']['sampling'];
  $action['format'] = $form_state['values']['format'];
  $action['fps'] = $form_state['values']['fps'];
  $action['start_time'] = $form_state['values']['start_time'];
  $action['duration'] = $form_state['values']['duration'];
  $action = serialize($action);
  //save
  if (empty($actionid)) {
    simplevideo_enc_action_save($presetid, $action);
  }
  //update
  else {
    simplevideo_update_enc_settings($actionid, $action);
  }
}

function simplevideo_ui_thumbnails_form($form_state, $presetid = NULL, $actionid = NULL) {
  $settings = simplevideo_get_thumb_settings($actionid);
  $form = array();

  $form['presetid'] = array(
      '#type' => 'value',
      '#value' => $presetid,
  );

  $form['actionid'] = array(
      '#type' => 'value',
      '#value' => $actionid,
  );

  $form['width'] = array(
      '#type' => 'textfield',
      '#title' => t('Width'),
      '#description' => t('width thumbnails'),
      '#size' => 20,
      '#default_value' => $settings['thumb']['width'],
      '#required' => TRUE
  );
  $form['height'] = array(
      '#type' => 'textfield',
      '#title' => t('Height'),
      '#description' => t('height thumbnails'),
      '#size' => 20,
      '#default_value' => $settings['thumb']['height'],
      '#required' => TRUE
  );
  $form['count'] = array(
      '#type' => 'textfield',
      '#title' => t('Count'),
      '#description' => t('Count thumbnails'),
      '#size' => 10,
      '#default_value' => $settings['thumb']['count'],
      '#required' => TRUE
  );
  $form['time'] = array(
      '#type' => 'textfield',
      '#title' => t('Start time'),
      '#description' => t('Input format: hh:mm:ss'),
      '#size' => 10,
      '#default_value' => $settings['thumb']['time'],
      '#required' => TRUE,
      '#element_validate' => array('simplevideo_ui_time_validate'),
  );
  $form['delay'] = array(
      '#type' => 'textfield',
      '#title' => t('Delay'),
      '#description' => t('Delay between thumbnails'),
      '#size' => 10,
      '#default_value' => $settings['thumb']['delay'],
      '#required' => TRUE
  );
  $form['save'] = array(
      '#type' => 'submit',
      '#value' => t('Save action'),
  );

  $form["#validate"][] = "simplevideo_ui_alpha_numeric_validate";

  return $form;
}

function simplevideo_ui_time_validate($form, &$form_state) {
  $element_validate = array('time', 'start_time', 'duration');
  foreach ($form_state['values'] as $key => $value) {
    if (array_search($key, $element_validate) && $value) {
      $regexp = ('/^([0-2]?(?<=[0-2]{1})[0-3]{1}|[0-2]? (?<=[0-1]{1})\d{1}):([0-5]{1}\d{1})(?:\:(?=\d{1})([0-5]{1}(?<=\d{1})\d{1}))?$/');
        if (!preg_match($regexp, $value, $matches)) {
          form_set_error($key, t('Enter time in format hh:mm:ss'));
        }
    $form_state['time']['hour'] = $matches[1];
    $form_state['time']['min'] = $matches[2];
    $form_state['time']['sec'] = $matches[3];
    }
  }
}

function simplevideo_ui_thumbnails_form_submit($form, &$form_state) {
  $presetid = $form_state['values']['presetid'];
  $actionid = $form_state['values']['actionid'];
  $form_state['redirect'] = 'admin/build/simplevideo/'. $presetid;
  $seconds = $form_state['values']['time'];
  $action = array();
  $action['seconds'] = $seconds;
  $action['width'] = $form_state['values']['width'];
  $action['height'] = $form_state['values']['height'];
  $action['count'] = $form_state['values']['count'];
  $action['delay'] = $form_state['values']['delay'];
  $action = serialize($action);
  //save
  if (empty($actionid)) {
    simplevideo_thumb_action_save($presetid, $action);
  }
  //update
  else {
    simplevideo_update_thumb_settings($actionid, $action);
  }
}

function simplevideo_ui_thumbnails_form_validate($form, &$form_state) {
  $presetid = $form_state['values']['presetid'];
  $actionid = $form_state['values']['actionid'];
  $result = db_query("SELECT thumb_width, thumb_height FROM {simplevideo_thumb_action} WHERE actionid =%d", $actionid);
  while ($row = db_fetch_array($result)) {
    $width = $row['width'];
    $height = $row['height'];
  }
  //if save
  if (empty($actionid)) {
    $result = db_query("SELECT actionid FROM {simplevideo_enc_action} WHERE presetid =%d", $presetid);
    while ($row = db_fetch_array($result)) {
      $actionid_enc[] = $row['actionid'];
    }
    $result = db_query("SELECT actionid FROM {simplevideo_thumb_action} WHERE presetid =%d", $presetid);
    while ($row = db_fetch_array($result)) {
      $actionid_thumb[] = $row['actionid'];
    }
    //Not 2 actions in the preset
    if (count($actionid_enc) >= 1 || count($actionid_thumb) >= 1 ) {
      form_set_error('simplevideo_ui_thumbnails_form', t('In the preset already have a action, please create a new preset'));
    }
  }
}

/**
 * List action to preset
 */
function theme_simplevideo_ui_preset_actions($form) {
  $presetid = $form['presetid']['#value'];
  $header = array(t('Action'), t('Settings'), t('Edit'));
  $result = db_query("SELECT parameters FROM {simplevideo_enc_action} WHERE presetid = %d", $presetid);
  $rows = array();
  while ($presets = db_fetch_array($result)) {
    $action = unserialize($presets['parameters']);
    $row = array();
    $row[] = t('Encoding video');
    $links = array();
    $settings = array();
    $settings[] = t('Width: !width', array('!width' => $action['width']));
    $settings[] = t('Height: !height', array('!height' => $action['height']));
    $settings[] = t('Video bitrate: !v_bitrate', array('!v_bitrate' => $action['bitrate']));
    $settings[] = t('Audio bitrate: !a_bitrate', array('!a_bitrate' => $action['a_bitrate']));
    $settings[] = t('Sampling: !sampling', array('!sampling' => $action['sampling']));
    $links[] = l(t('Configure'), 'admin/build/simplevideo/'. $presetid .'/enc/'. $presets['actionid'] );
    $links[] = l(t('Delete'), 'admin/build/simplevideo/'. $presetid .'/enc/'. $presets['actionid'] .'/delete');
    $row[] = implode('&nbsp;&nbsp;&nbsp;&nbsp;', $settings);
    $row[] = implode('&nbsp;&nbsp;&nbsp;&nbsp;', $links);
    $rows[] = $row;
  }
  $result = db_query("SELECT actionid, parameters FROM {simplevideo_thumb_action} WHERE presetid = %d", $presetid);
  while ($presets = db_fetch_array($result)) {
    $action = unserialize($presets['parameters']);
    $row = array();
    $row[] = t('Create thumbnails');
    $links = array();
    $settings = array();
    $settings[] = t('Width: !width', array('!width' => $action['width']));
    $settings[] = t('Height: !height', array('!height' => $action['height']));
    $settings[] = t('Count: !count', array('!count' => $action['count']));
    $settings[] = t('Start time: !time', array('!time' => $action['seconds']));
    $settings[] = t('Delay: !delay', array('!delay' => $action['delay']));
    $links[] = l(t('Configure'), 'admin/build/simplevideo/'. $presetid .'/thumb/'. $presets['actionid'] );
    $links[] = l(t('Delete'), 'admin/build/simplevideo/'. $presetid .'/thumb/'. $presets['actionid'] .'/delete');
    $row[] = implode('&nbsp;&nbsp;&nbsp;&nbsp;', $settings);
    $row[] = implode('&nbsp;&nbsp;&nbsp;&nbsp;', $links);
    $rows[] = $row;
  }
  $output = theme('table', $header, $rows);
  $output .= drupal_render($form);
  return $output;
}

function simplevideo_ui_players_callback($arg = NULL) {
  return drupal_get_form('simplevideo_ui_players_form', array(), $arg);
}

function simplevideo_ui_players_form(&$form_state, $item = array(), $arg = NULL) {
  $playerid = ($arg == NULL) ? $form_state['values']['playerid'] : $arg;

  // AHAH for variables
  if (empty($playerid)) { //if create new player
    $options = array();
    $options_count = 0;
    if (isset($form_state['post']['options'])) {
      $options = $form_state['post']['options'];
      $options_count = count($form_state['post']['options']) + 1;
    }
    else {
      $options = $item;
      $options_count = empty($options) ? 1 : count($options);
    }
  }
  else { // if Edit player
    $player = simplevideo_get_player_settings($playerid);
    $array = $player['options'];
    $options_count = count($array);
    if (isset($form_state['post']['options'])) {
      $options = $form_state['post']['options'];
      $options_count = count($form_state['post']['options']) + 1;
    }
  }

  // AHAH for parameters
  if (empty($playerid)) { // if create
    $settings = array();
    $settings_count = 0;
    if (isset($form_state['post']['settings'])) {
      $settings = $form_state['post']['settings'];
      $settings_count = count($form_state['post']['settings']) + 1;
    }
    else {
      $settings = $item;
      $settings_count = empty($settings) ? 1 : count($settings);
    }
  }
  else { //if edit
    $array_settings = $player['parameters'];
    $settings_count = count($array_settings);
    if (isset($form_state['post']['settings'])) {
      $settings = $form_state['post']['settings'];
      $settings_count = count($form_state['post']['settings']) + 1;
    }
  }

  $form = array();

  $form['playerid'] = array(
      '#type' => 'hidden',
      '#value' => $playerid,
  );
  $form['path'] = array(
      '#type' => 'textfield',
      '#title' => t('File player path'),
      '#description' => t('Path to player'),
      '#size' => 100,
      '#default_value' => $player['path'],
  );
  $form['machinename'] = array(
      '#type' => 'textfield',
      '#title' => t('Machine-readable name'),
      '#description' => t('Formatter machine name'),
      '#size' => 36,
      '#default_value' => $player['machine_name'],
      //'#element_validate' => array('simplevideo_ui_player_machinename_validate'),
  );
  $form['humanname'] = array(
      '#type' => 'textfield',
      '#title' => t('Human-readable name'),
      '#description' => t('Formatter personal name'),
      '#size' => 36,
      '#default_value' => $player['personal_name'],
  );
  $form['settings'] = array(
      '#type' => 'fieldset',
      '#title' => t('Parameters'),
      '#tree' => TRUE,
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
  );
  $form['settings']['settings_wrapper'] = array(
      '#type' => 'markup',
      '#prefix' => '<div id = "settings-wrapper">',
      '#suffix' => '</div>',
  );
  for ($j = 0; $j < $settings_count; $j++) {
    $form['settings']['settings_wrapper']["inline_$j"] = array(
        '#type' => 'markup',
        '#prefix' => '<div class="container-inline" id = "inline">',
        '#suffix' => '</div>',
    );
    $form['settings']['settings_wrapper']["inline_$j"]["key_$j"] = array(
        '#type' => 'textfield',
        '#title' => t("Key param"),
        '#size' => 20,
        '#name' => "settings[$j][key]",
    );
    $form['settings']['settings_wrapper']["inline_$j"]["value_$j"] = array(
        '#type' => 'textfield',
        '#title' => t("Value param"),
        '#size' => 20,
        '#name' => "settings[$j][value]",
    );
    if (empty($playerid)) { // if create
      $form['settings']['settings_wrapper']["inline_$j"]["key_$j"]['#default_value'] = $settings[$j]['key'];
      $form['settings']['settings_wrapper']["inline_$j"]["value_$j"]['#default_value'] = $settings[$j]['value'];
    }
    else { // if edit
      if (!empty($array_settings[$j]['key']) || !empty($array_settings[$j]['value'])) {
        $form['settings']['settings_wrapper']["inline_$j"]["key_$j"]['#default_value'] = $array_settings[$j]['key'];
        $form['settings']['settings_wrapper']["inline_$j"]["value_$j"]['#default_value'] = $array_settings[$j]['value'];
      }
      if (empty($array_settings[$j]['key']) || empty($array_settings[$j]['value'])) {
        $form['settings']['settings_wrapper']["inline_$j"]["key_$j"]['#default_value'] = $settings[$j]['key'];
        $form['settings']['settings_wrapper']["inline_$j"]["value_$j"]['#default_value'] = $settings[$j]['value'];
      }
    }
  }
  $form['settings']['add'] = array(
      '#type' => 'button',
      '#value' => t('Add new parameter'),
      '#weight' => '100',
      '#ahah' => array(
      'path' => 'simplevideo/js/settings',
      'wrapper' => 'settings-wrapper',
      'method' => 'replace',
      'event' => 'click',
      'effect' => 'fade',
      ),
  );
  $form['param'] = array(
      '#type' => 'fieldset',
      '#title' => t('Variables'),
      '#tree' => TRUE,
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
  );
  $form['param']['param_wrapper'] = array(
      '#type' => 'markup',
      '#prefix' => '<div id = "param-wrapper">',
      '#suffix' => '</div>',
  );
  for ($i = 0; $i < $options_count; $i++) {
    $form['param']['param_wrapper']["inline_$i"] = array(
        '#type' => 'markup',
        '#prefix' => '<div class="container-inline" id = "inline">',
        '#suffix' => '</div>',
    );
    $form['param']['param_wrapper']["inline_$i"]["key_$i"] = array(
        '#type' => 'textfield',
        '#title' => t("Key param"),
        '#size' => 20,
        '#name' => "options[$i][key]",
    );
    $form['param']['param_wrapper']["inline_$i"]["value_$i"] = array(
        '#type' => 'textfield',
        '#title' => t("Value param"),
        '#size' => 20,
        '#name' => "options[$i][value]",
    );
    if (empty($playerid)) { // if create
      $form['param']['param_wrapper']["inline_$i"]["key_$i"]['#default_value'] = $options[$i]['key'];
      $form['param']['param_wrapper']["inline_$i"]["value_$i"]['#default_value'] = $options[$i]['value'];
    }
    else { // if edit
      if (!empty($array[$i]['key']) || !empty($array[$i]['value'])) {
        $form['param']['param_wrapper']["inline_$i"]["key_$i"]['#default_value'] = $array[$i]['key'];
        $form['param']['param_wrapper']["inline_$i"]["value_$i"]['#default_value'] = $array[$i]['value'];
      }
      if (empty($array[$i]['key']) || empty($array[$i]['value'])) {
        $form['param']['param_wrapper']["inline_$i"]["key_$i"]['#default_value'] = $options[$i]['key'];
        $form['param']['param_wrapper']["inline_$i"]["value_$i"]['#default_value'] = $options[$i]['value'];
      }
    }
  }
  $form['param']['add'] = array(
      '#type' => 'button',
      '#value' => t('Add new variable'),
      '#weight' => '100',
      '#ahah' => array(
      'path' => 'simplevideo/js/var',
      'wrapper' => 'param-wrapper',
      'method' => 'replace',
      'event' => 'click',
      'effect' => 'fade',
      ),
  );

  $form['save'] = array(
      '#type' => 'submit',
      '#value' => t('Save'),
      '#weight' => '101',
  );

  $form['view_token'] = array(
      '#value' => theme('token_help', 'simplevideo'),
      '#weight' => '102',
  );

  return $form;
}

function simplevideo_ui_player_machinename_validate($form, &$form_state) {
  $machinename = $form_state['values']['machinename'];
  $players = simplevideo_players();
  $players = $players['machinename'];
  // Check for duplicates
  foreach ($players as $value) {
    if ($value == $machinename) {
      form_set_error('machinename', t('The players machinename name you have chosen is already in use.'));
      break;
    }
  }

  // Check for illegal characters in preset names
  if (preg_match('/[^0-9a-zA-Z_\-]/', $machinename)) {
    form_set_error('machinename', t('Please only use alphanumeric characters, underscores (_), and hyphens (-) for preset names.'));
  }

}

function simplevideo_ui_player_delete_form($form_state, $playerid = NULL) {
  $form = array();
  $form['playerid'] = array(
    '#type' => 'value',
    '#value' => $playerid,
  );
  $question = t("Are you sure you want to delete the player");
  return confirm_form($form, $question, 'admin/build/simplevideo/playerslist',
  t('This action cannot be undone.'), t('Delete'),  t('Cancel'));
}

function simplevideo_ui_player_delete_form_submit($form, &$form_state) {
  $playerid = $form_state['values']['playerid'];
  $form_state['redirect'] = 'admin/build/simplevideo/playerslist/';
  simplevideo_delete_player($playerid);
}

/**
 * Menu callback for AHAH additions.
 */
function  simplevideo_ui_choice_js($arg) {
  $form_state = array('storage' => NULL, 'submitted' => FALSE);
  $form_build_id = $_POST['form_build_id'];
  // Get the form from the cache.
  $form = form_get_cache($form_build_id, $form_state);
  $args = $form['#parameters'];
  $form_id = array_shift($args);
  // We will run some of the submit handlers so we need to disable redirecting.
  $form['#redirect'] = FALSE;
  // We need to process the form, prepare for that by setting a few internals
  // variables.
  $form['#post'] = $_POST;
  $form['#programmed'] = FALSE;
  $form_state['post'] = $_POST;
  $form_state_copy = $form_state;
  drupal_process_form($form_id, $form, $form_state_copy);
  $form = drupal_rebuild_form($form_id, $form_state_copy, $args, $form_build_id);
  if ($arg == 'var') {
    $new_form = $form['param']['param_wrapper'];
  }
  if ($arg == 'settings') {
    $new_form = $form['settings']['settings_wrapper'];
  }
  $output = theme('status_messages') . drupal_render($new_form);
  drupal_json(array('status' => TRUE, 'data' => $output));
}

function simplevideo_ui_players_form_submit($form, &$form_state) {
  $playerid = $form_state['values']['playerid'];
  $path = $form_state['values']['path'];
  $machine_name = $form_state['values']['machinename'];
  $personal_name = $form_state['values']['humanname'];
  $options  = $form_state["clicked_button"]["#post"]["options"];
  $param = $form_state["clicked_button"]["#post"]["settings"];
  simplevideo_save_or_update_players($path, $machine_name, $personal_name, $options, $param, $playerid);
  simplevideo_cache_clear();
  $form_state['redirect'] = 'admin/build/simplevideo/playerslist/';
}

/**
 * List players
 */
function simplevideo_ui_players_overview() {
  $header = array(t('Player name'), t('Actions'));
  $result = db_query("SELECT playerid, personalname FROM {simplevideo_players}");
  $rows = array();
  while ($players = db_fetch_array($result)) {
    $row = array();
    $row[] = l($players['personalname'], 'admin/build/simplevideo/playerslist/'. $players['playerid']);
    $links = array();
    $links[] = l(t('Clone'), 'admin/build/simplevideo/playerslist/'. $players['playerid'] .'/clone');
    $links[] = l(t('Edit'), 'admin/build/simplevideo/playerslist/'. $players['playerid']);
    $links[] = l(t('Delete'), 'admin/build/simplevideo/playerslist/'. $players['playerid'] .'/delete');
    $row[] = implode('&nbsp;&nbsp;&nbsp;&nbsp;', $links);
    $rows[] = $row;
  }
  $output = theme('table', $header, $rows);
  return $output;
}

function simplevideo_ui_alpha_numeric_validate($form, &$form_state) {
  $exclude = array('op', 'save', 'form_build_id', 'form_token', 'form_id', 'time', 'start_time', 'duration');
  foreach ($form_state['values'] as $key => $value) {
    if (array_search($key, $exclude) === FALSE) {
      if (preg_match('/[^0-9a-zA-Z_\-]/', $value)) {
        form_set_error($key, t('Please only use alphanumeric characters, underscores (_), and hyphens (-)'));
      }
    }
  }
}

function simplevideo_ui_clone_form($form_state, $id) {
  $form = array();
  $submitter = array('simplevideo_clone');
  $form['#redirect'] = 'admin/build/simplevideo/playerslist';
  $form['new_personalname'] = array(
          '#type' => 'textfield',
          '#title' => t('Human-readable name'),
          '#description' => t('Plase enter new human-readable name to clone object'),
          '#required' => TRUE,
  );
  $form['new_machinename'] = array(
          '#type' => 'textfield',
          '#title' => t('Machine-readable name'),
          '#description' => t('Plase enter new machine-readable name to clone object'),
          '#required' => TRUE,
  );
  $form['id'] = array(
          '#type' => 'value',
          '#value' => $id
  );
  $form['save'] = array(
          '#type' => 'submit',
          '#value' => t('Save'),
          '#submit' => $submitter,
  );

  return $form;
}