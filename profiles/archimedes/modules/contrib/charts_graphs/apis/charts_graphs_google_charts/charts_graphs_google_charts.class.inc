<?php
// $Id: charts_graphs_google_charts.class.inc,v 1.1.2.13 2010/11/18 01:52:04 rsevero Exp $

/**
 * @file
 *   Implementation of abstract class ChartsGraphsCanvas for Google Charts library.
 *
 */

define('CHARTS_GRAPHS_GOOGLE_CHARTS_AREA_MARKER_STYLE', 'B');
define('CHARTS_GRAPHS_GOOGLE_CHARTS_DATA_STARTER', 't:');

define('CHARTS_GRAPHS_GOOGLE_CHARTS_DATA_PARAMETER', 'chd');
define('CHARTS_GRAPHS_GOOGLE_CHARTS_MIN_MAX_PER_SERIE_PARAMETER', 'chds');
define('CHARTS_GRAPHS_GOOGLE_CHARTS_VISIBLE_AXIS_PARAMETER', 'chxt');
define('CHARTS_GRAPHS_GOOGLE_CHARTS_AXIS_RANGE_PARAMETER', 'chxr');
define('CHARTS_GRAPHS_GOOGLE_CHARTS_CUSTOM_AXIS_LABELS', 'chxl');
define('CHARTS_GRAPHS_GOOGLE_CHARTS_CHART_TYPE', 'cht');
define('CHARTS_GRAPHS_GOOGLE_CHARTS_MARKER_STYLE', 'chm');
define('CHARTS_GRAPHS_GOOGLE_CHARTS_CHART_TITLE', 'chtt');
define('CHARTS_GRAPHS_GOOGLE_CHARTS_CHART_TITLE_COLOR', 'chts');
define('CHARTS_GRAPHS_GOOGLE_CHARTS_CHART_SIZE', 'chs');
define('CHARTS_GRAPHS_GOOGLE_CHARTS_SERIES_COLOUR', 'chco');
define('CHARTS_GRAPHS_GOOGLE_CHARTS_LABEL_POSITION', 'chxp');
define('CHARTS_GRAPHS_GOOGLE_CHARTS_BARS_WIDTH_SPACING', 'chbh');
define('CHARTS_GRAPHS_GOOGLE_CHARTS_LEGEND', 'chdl');
define('CHARTS_GRAPHS_GOOGLE_CHARTS_LEGEND_POSITION', 'chdlp');
define('CHARTS_GRAPHS_GOOGLE_CHARTS_BACKGROUND_COLOUR', 'chf');

define('CHARTS_GRAPHS_GOOGLE_CHARTS_COLOUR_DEFINITION_LENGTH', 6);

define('CHARTS_GRAPHS_GOOGLE_CHARTS_URL_PREFIX', 'http://chart.apis.google.com/chart?');

/**
 * Implementation of abstract class ChartsGraphsCanvas for Google Charts library.
 */
class ChartsGraphsGoogleCharts extends ChartsGraphsCanvas {

  var $width = 450;
  var $height = 200;
  var $title = '';

  /**
   * Sets how we count how many colours must be sent.
   *
   * TRUE: counts the number of data series available
   * FALSE: counts the munber of data poitns in the first serie. This last
   * behaviour is used with pie charts.
   * 
   * @var <bool>
   */
  var $colour_count_series = TRUE;
  
  /**
   * Parameters set directly by the user.
   *
   * @var <array> 
   */
  var $parameters = array();

  protected function _encode_chart_type() {
    switch ($this->type) {
      case 'line':
        $type = 'lc';
        break;

      case 'area':
        $type = 'lc';
        break;

      case 'bar':
        $type = 'bvg';
        break;

      case 'pie':
        $type = 'p';
        $this->colour_count_series = FALSE;
        break;

      case 'side_bar':
        $type = 'bhg';
        break;

      case 'queued_bar':
        $type = 'bvo';
        break;

       case 'stacked_bar':
         $type = 'bvs';
         break;

       case 'stacked_side_bar':
         $type = 'bhs';
         break;

       case 'pie_3d':
         $type = 'p3';
         $this->colour_count_series = FALSE;
         break;
    }
    $this->parameters_to_send[CHARTS_GRAPHS_GOOGLE_CHARTS_CHART_TYPE] = $type;
  }

  protected function _encode_data() {
    $series = $this->series;
    $is_stacked = (strpos($this->type, 'stacked') !== FALSE);
    // Use zero as initial min to guarantee that the y axis show zero.
    $min_y = 0;
    $max_y = $is_stacked ? 0 : reset(reset($series));
    $chds = '';
    $chd = array();
    $chdl = array();
    foreach ($series as $serie_name => $serie) {
      $chdl[] = $serie_name;

      $serie_as_string = implode(',',$serie);
      
      if (min($serie) < $min_y) {
        $min_y = min($serie);
      }
      if ($is_stacked) {
        $max_y += max($serie);
      }
      else {
        if (max($serie) > $max_y) {
          $max_y = max($serie);
        }
      }

      /**
       * Applying user defined min and max y axis values.
       */
      if (isset($this->y_min)) {
        $min_y = $this->y_min;
      }
      if (isset($this->y_max)) {
        $max_y = $this->y_max;
      }
      $chd[] = $serie_as_string;
    }
    $chds = drupal_urlencode($min_y) . ',' . drupal_urlencode($max_y);

    $this->parameters_to_send[CHARTS_GRAPHS_GOOGLE_CHARTS_DATA_PARAMETER] = 
      CHARTS_GRAPHS_GOOGLE_CHARTS_DATA_STARTER . implode('|', $chd);
    $this->parameters_to_send[CHARTS_GRAPHS_GOOGLE_CHARTS_MIN_MAX_PER_SERIE_PARAMETER] = $chds;
    $this->parameters_to_send[CHARTS_GRAPHS_GOOGLE_CHARTS_VISIBLE_AXIS_PARAMETER] = 'x,y';
    $this->parameters_to_send[CHARTS_GRAPHS_GOOGLE_CHARTS_LEGEND] = implode('|', $chdl);
    if (strpos($this->type, 'side') === FALSE) {
      $scale_axis_identifier = 1;
      $text_labels_axis_identifier = 0;
      $y_legend_axis = ',y';
    }
    else {
      $scale_axis_identifier = 0;
      $text_labels_axis_identifier = 1;
      $y_legend_axis = ',x';
    }
    if ($this->y_legend) {
      $this->parameters_to_send[CHARTS_GRAPHS_GOOGLE_CHARTS_VISIBLE_AXIS_PARAMETER] .= $y_legend_axis;
      $y_legend = '2:|' . drupal_urlencode($this->y_legend) . '|';
      $this->parameters_to_send[CHARTS_GRAPHS_GOOGLE_CHARTS_LABEL_POSITION] = '2,50';
    }
    else {
      $y_legend = '';
    }
    $this->parameters_to_send[CHARTS_GRAPHS_GOOGLE_CHARTS_CUSTOM_AXIS_LABELS] =
      $y_legend . $text_labels_axis_identifier . ':' . $this->_get_encoded_text_labels();

    /**
     * Applying user defined min and max y axis values.
     */
    if (isset($this->y_min)) {
      $min_y = $this->y_min;
    }
    if (isset($this->y_max)) {
      $max_y = $this->y_max;
    }
    $this->parameters_to_send[CHARTS_GRAPHS_GOOGLE_CHARTS_AXIS_RANGE_PARAMETER] =
      $scale_axis_identifier . ',' . drupal_urlencode($min_y) . ',' .
      drupal_urlencode($max_y);
    /**
     * Applying user defined step for y axis values.
     */
    if (isset($this->y_step)) {
      $this->parameters_to_send[CHARTS_GRAPHS_GOOGLE_CHARTS_AXIS_RANGE_PARAMETER] .= ',' . $this->y_step;
    }

    $colour_count = $this->colour_count_series ? count($series) : count(reset($series));
    $colours = array_slice($this->series_colours(), 0, $colour_count);
    $pure_hex_colours = array();
    foreach ($colours as $colour) {
      $pure_hex_colours[] = substr($colour, -6);
    }
    $this->parameters_to_send[CHARTS_GRAPHS_GOOGLE_CHARTS_SERIES_COLOUR] = implode(',', $pure_hex_colours);
    if ($this->type == 'area') {
      $fill_colour = array();
      $i = 0;
      foreach ($pure_hex_colours as $colour) {
        $fill_colour[] = sprintf(
          '%s,%s,%u,0,0',
          CHARTS_GRAPHS_GOOGLE_CHARTS_AREA_MARKER_STYLE,
          $colour,
          $i
        );
        $i++;
      }
      $this->parameters_to_send[CHARTS_GRAPHS_GOOGLE_CHARTS_MARKER_STYLE] = implode('|', $fill_colour);
    }
    if ($this->title_color) {
      $this->parameters_to_send[CHARTS_GRAPHS_GOOGLE_CHARTS_CHART_TITLE_COLOR] = $this->title_color;
    }
    if ($this->label_pos) {
      $chdlp = "";
      switch ($this->label_pos) {
        case 'top':
          $chdlp = "t";
          break;
        case 'bottom':
          $chdlp = "b";
          break;
        case 'top_vert':
          $chdlp = "tv";
          break;
        case 'bottom_vert':
          $chdlp = "bv";
          break;
        case 'left':
          $chdlp = "l";
          break;
        default:
          $chdlp = "r";
      }
      $this->parameters_to_send[CHARTS_GRAPHS_GOOGLE_CHARTS_LEGEND_POSITION] = $chdlp;
    }
  }

  protected function _get_encoded_text_labels() {
    $chxl = '';
    if (is_array($this->x_labels)) {
      /**
       * Workaround a apparently Google Charts bug: data labels are inverted on
       * side bar graphs.
       */
      $x_labels = ($this->type == 'side_bar') ?
        array_reverse($this->x_labels) :
        $this->x_labels;
      foreach ($x_labels as $label) {
        $chxl .= '|' . drupal_urlencode($label);
      }
    }
    return $chxl;
  }
  
  protected function _encode_other_parameters() {
    $this->parameters_to_send[CHARTS_GRAPHS_GOOGLE_CHARTS_CHART_TITLE] = drupal_urlencode($this->title);
    $this->parameters_to_send[CHARTS_GRAPHS_GOOGLE_CHARTS_CHART_SIZE] = drupal_urlencode(sprintf(
      '%ux%u',
      $this->width,
      $this->height
    ));
    if (strpos($this->type, 'bar') !== FALSE) {
      $this->parameters_to_send[CHARTS_GRAPHS_GOOGLE_CHARTS_BARS_WIDTH_SPACING] = 'a';
    }

    /**
     * Applying background colour setting if available.
     */
    if (isset($this->colour) && !empty($this->colour)) {
      $this->parameters_to_send[CHARTS_GRAPHS_GOOGLE_CHARTS_BACKGROUND_COLOUR] = sprintf(
        'bg,s,%s',
        substr($this->colour, -(CHARTS_GRAPHS_GOOGLE_CHARTS_COLOUR_DEFINITION_LENGTH))
      );
    }
  }

  /**
   * Pushes user defined parameters over any parameters defined by the class.
   *
   * I.e., the user defined parameters overwrite any parameters defined
   * elsewhere. We don't make any processing on these parameters. They are
   * outputed exactly as sent. The user is responsable for urlenconding this
   * data.
   *
   * If the user wants to simply unset some parameter, it can do it senting the
   * parameter with a NULL value.
   */
  protected function _encode_user_parameters() {
    if (is_array($this->parameters)) {
      foreach ($this->parameters as $key => $value) {
        if ($value === NULL) {
          unset($this->parameters_to_send[$key]);
        }
        else {
          $this->parameters_to_send[$key] = $value;
        }
      }
    }
  }

  protected function _get_encoded_value($value) {
    if (is_array($value)) {
      $encoded_value = '';
      foreach ($value as $val) {
        $encoded_value .= '|' . $this->_get_encoded_value($val);
      }
      if (strlen($encoded_value)) {
        $encoded_value = substr($encoded_value, 1);
      }
    }
    else {
      $encoded_value = drupal_urlencode($value);
    }
    return $encoded_value;
  }

  protected function _get_query() {
    $query = '';
    foreach ($this->parameters_to_send as $key => $value) {
      $query .= '&' . $key . '=' . $value;
    }
    if (strlen($query)) {
      $query = substr($query, 1);
    }
    return $query;
  }

  protected function _get_url() {
    $this->parameters_to_send = array();
    $this->_encode_chart_type();
    $this->_encode_data();
    $this->_encode_other_parameters();
    $this->_encode_user_parameters();
    $url = CHARTS_GRAPHS_GOOGLE_CHARTS_URL_PREFIX . $this->_get_query();
    return $url;
  }

  /**
   * Function that renders data.
   */
  public function get_chart() {
    $provider = charts_graphs_google_charts_chartgraph_provider();
    $chart_id = sprintf(
      '%s-chart-%d',
      $this->type,
      $this->getUnique_ID()
    );
    $alt_text = sprintf(
      '%s %s chart',
      $this->title,
      $provider->chart_types[$this->type]
    );

    $output = sprintf(
      '<div id="%s-wrapper" class="charts_graphs_google_chart charts_graphs_google_chart_%s">
        <img id="%1$s" src="%s" alt="%s" />
      </div>',
      check_plain($chart_id),
      check_plain($this->type),
      $this->_get_url(),
      check_plain($alt_text)
    );
    return $output;
  }
}
