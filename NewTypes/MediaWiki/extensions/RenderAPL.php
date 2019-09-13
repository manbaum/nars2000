<?php

// MediaWiki extension to apply a consistent style to APL expressions
//   by defining a new tag name <apl>...</apl>.
// Save this file on the server in /rul/mediawiki/extensions/.

$wgHooks['ParserFirstCallInit'][] = 'wfAPLExtension';

$FontName = 'SImPL';

$md_style = "color: red; background: yellow; text-decoration: underline;";

function wfAPLExtension (Parser $parser)
{
    // Register the extension with the WikiText parser
    // the first parameter is the name of the new tag.
    // In this case it defines the tag <apl> ... </apl>
    // the second parameter is the callback function for
    // processing the text between the tags
    $parser->setHook( 'apl'  , 'renderAPL'        );
    $parser->setHook( 'apll' , 'renderAPLlarge'   );
    $parser->sethook( 'aplx' , 'renderAPLxlarge'  );
    $parser->sethook( 'aplxx', 'renderAPLxxlarge' );

    $parser->sethook( 'pn'   , 'pnNotation'       );
    $parser->sethook( 'hc'   , 'hcNotation'       );
    $parser->sethook( '_mark', 'mark_digit'       );

    $parser->sethook( '_ad'  , 'pn_ad_Notation'   );
    $parser->sethook( '_ah'  , 'pn_ah_Notation'   );
    $parser->sethook( '_ar'  , 'pn_ar_Notation'   );
    $parser->sethook( '_au'  , 'pn_au_Notation'   );
    $parser->sethook( '_b'   , 'pn_b_Notation'    );
    $parser->sethook( '_e'   , 'pn_e1_Notation'   );
    $parser->sethook( '_E'   , 'pn_E2_Notation'   );
    $parser->sethook( '_g'   , 'pn_g_Notation'    );
    $parser->sethook( '_i'   , 'pn_i_Notation'    );
    $parser->sethook( '_j'   , 'pn_j1_Notation'   );
    $parser->sethook( '_J'   , 'pn_J2_Notation'   );
    $parser->sethook( '_k'   , 'pn_k_Notation'    );
    $parser->sethook( '_l'   , 'pn_l_Notation'    );
    $parser->sethook( '_ij'  , 'pn_ij_Notation'   );
    $parser->sethook( '_jk'  , 'pn_jk_Notation'   );
    $parser->sethook( '_kl'  , 'pn_kl_Notation'   );
    $parser->sethook( '_lc'  , 'pn_lc_Notation'   );
    $parser->sethook( '_os'  , 'pn_os_Notation'   );
    $parser->sethook( '_oi'  , 'pn_oi_Notation'   );
    $parser->sethook( '_oj'  , 'pn_oj_Notation'   );
    $parser->sethook( '_ok'  , 'pn_ok_Notation'   );
    $parser->sethook( '_p'   , 'pn_p_Notation'    );
    $parser->sethook( '_pom' , 'pn_pom_Notation'  );
    $parser->sethook( '_r'   , 'pn_r_Notation'    );
    $parser->sethook( '_s'   , 'pn_s_Notation'    );
    $parser->sethook( '_v'   , 'pn_v_Notation'    );
    $parser->sethook( '_x'   , 'pn_x_Notation'    );

    $parser->sethook( 'ad'   , 'pn_ad_Notation'   );
    $parser->sethook( 'ar'   , 'pn_ar_Notation'   );
    $parser->sethook( 'au'   , 'pn_au_Notation'   );
    $parser->sethook( 'bn'   , 'pn_b_Notation'    );
    $parser->sethook( 'e'    , 'pn_e1_Notation'   );
    $parser->sethook( 'E'    , 'pn_E2_Notation'   );
    $parser->sethook( 'g'    , 'pn_g_Notation'    );
    $parser->sethook( 'in'   , 'pn_i_Notation'    );
    $parser->sethook( 'j'    , 'pn_j1_Notation'   );
    $parser->sethook( 'J'    , 'pn_J2_Notation'   );
    $parser->sethook( 'k'    , 'pn_k_Notation'    );
    $parser->sethook( 'l'    , 'pn_l_Notation'    );
    $parser->sethook( 'ij'   , 'pn_ij_Notation'   );
    $parser->sethook( 'jk'   , 'pn_jk_Notation'   );
    $parser->sethook( 'kl'   , 'pn_kl_Notation'   );
    $parser->sethook( 'os'   , 'pn_os_Notation'   );
    $parser->sethook( 'oi'   , 'pn_oi_Notation'   );
    $parser->sethook( 'oj'   , 'pn_oj_Notation'   );
    $parser->sethook( 'ok'   , 'pn_ok_Notation'   );
    $parser->sethook( 'pi'   , 'pn_p_Notation'    );
    $parser->sethook( 'pom'  , 'pn_pom_Notation'  );
    $parser->sethook( 'r'    , 'pn_r_Notation'    );
    $parser->sethook( 's'    , 'pn_s_Notation'    );
    $parser->sethook( 'v'    , 'pn_v_Notation'    );
    $parser->sethook( 'x'    , 'pn_x_Notation'    );

    return true;
} // End wfAPLExtension


// The callback function for converting the input text to HTML output
function renderAPL ($input, array $args, Parser $parser, PPFrame $frame)
{
    return renderAPLcom ($input, $args, $parser, 0, false);
} // End renderAPL


// The callback function for converting the input text to HTML output
function renderAPLlarge ($input, array $args, Parser $parser, PPFrame $frame)
{
    return renderAPLcom ($input, $args, $parser, 1, true);
} // End renderAPLlarge


// The callback function for converting the input text to HTML output
function renderAPLxlarge ($input, array $args, Parser $parser, PPFrame $frame)
{
    return renderAPLcom ($input, $args, $parser, 2, true);
} // End renderAPLxlarge


// The callback function for converting the input text to HTML output
function renderAPLxxlarge ($input, array $args, Parser $parser, PPFrame $frame)
{
    return renderAPLcom ($input, $args, $parser, 3, true);
} // End renderAPLxxlarge


// The callback function for converting the input text to HTML output
function renderAPLcom ($input, array $argv, Parser $parser, $iLargeSize, $bBoldWeight)
{
    global $FontName, $md_style;

    // Disable caching for these parser tags
    $parser->disableCache ();

    // $argv is an array containing any arguments passed to the
    // extension like <example argument="foo" bar>..
    // Put this on the sandbox page:  (works in MediaWiki 1.5.5)
    //   <example argument="foo" argument2="bar">Testing text **example** in between the new tags</example>
    //    $output = "Text passed into example extension: <br/>$input";
    //    $output .= " <br/> and the value for the arg 'argument' is " . $argv["argument"];
    //    $output .= " <br/> and the value for the arg 'argument2' is: " . $argv["argument2"];
    //    return $output;

    $style = 'white-space: pre; background-color: lightcyan; ';

    if ($argv['size'])
        $style .= 'font-size: '   . $argv['size']   . '; ';
    else
    switch ($iLargeSize)
    {
        case 0:
            break;

        case 1:
            $style .= 'font-size: large; ';

            break;

        case 2:
            $style .= 'font-size: x-large; ';

            break;

        case 3:
            $style .= 'font-size: xx-large; ';

            break;
    } // End SWITCH

    if ($argv['font'])
        $style .= 'font-family: ' . $argv['font']   . '; ';
    else
        $style .= 'font-family: ' . $FontName       . '; ';

    if ($argv['weight'])
        $style .= 'font-weight: ' . $argv['weight'] . '; ';
    else
    if ($bBoldWeight)
        $style .= 'font-weight: bold; ';

    if ($argv['class'])
        $class = 'class="' . $argv['class'] . '" ';
    else
        $class = '';

    // Handle <pn>...</pn> within $input
    $input = preg_replace ('#<pn>(.*?)</pn>#', '<span style="color:red;">$1</span>', $input);

    // Handle <hc>...</hc> within $input
    $input = preg_replace ('#<hc>(.*?)</hc>#', '<span style="color:blue;">$1</span>', $input);

    // Handle <_mark>...</_mark> within $input
    $input = preg_replace ('#<_mark>(.*?)</_mark>#', '<span style="' . $md_style . '">$1</span>', $input);

    // Handle <_ad/> within $input
    $input = preg_replace ('#<ad */>#',   '<span style="color:blue;">ad</span>', $input);
    $input = preg_replace ('#<_ad */>#',  '<span style="color:blue;">ad</span>', $input);

    // Handle <_ah/> within $input
    $input = preg_replace ('#<_ah */>#',  '<span style="color:blue;">ah</span>', $input);

    // Handle <_ar/> within $input
    $input = preg_replace ('#<ar */>#',   '<span style="color:blue;">ar</span>', $input);
    $input = preg_replace ('#<_ar */>#',  '<span style="color:blue;">ar</span>', $input);

    // Handle <_au/> within $input
    $input = preg_replace ('#<au */>#',   '<span style="color:blue;">au</span>', $input);
    $input = preg_replace ('#<_au */>#',  '<span style="color:blue;">au</span>', $input);

    // Handle <_b/> within $input
    $input = preg_replace ('#<bn */>#',   '<span style="color:red;">b</span>', $input);
    $input = preg_replace ('#<_b */>#',   '<span style="color:red;">b</span>', $input);

    // Handle <_e/> within $input
    $input = preg_replace ('#<e */>#',    '<span style="color:red;">e</span>', $input);
    $input = preg_replace ('#<_e */>#',   '<span style="color:red;">e</span>', $input);

    // Handle <_E/> within $input
    $input = preg_replace ('#<E */>#',    '<span style="color:red;">E</span>', $input);
    $input = preg_replace ('#<_E */>#',   '<span style="color:red;">E</span>', $input);

    // Handle <_g/> within $input
    $input = preg_replace ('#<g */>#',    '<span style="color:red;">g</span>', $input);
    $input = preg_replace ('#<_g */>#',   '<span style="color:red;">g</span>', $input);

    // Handle <_i/> within $input
    $input = preg_replace ('#<in */>#',   '<span style="color:blue;">i</span>', $input);
    $input = preg_replace ('#<_i */>#',   '<span style="color:blue;">i</span>', $input);

    // Handle <_j/> within $input
    $input = preg_replace ('#<j */>#',    '<span style="color:blue;">j</span>', $input);
    $input = preg_replace ('#<_j */>#',   '<span style="color:blue;">j</span>', $input);

    // Handle <_J/> within $input
    $input = preg_replace ('#<J */>#',    '<span style="color:blue;">J</span>', $input);
    $input = preg_replace ('#<_J */>#',   '<span style="color:blue;">J</span>', $input);

    // Handle <_k/> within $input
    $input = preg_replace ('#<k */>#',    '<span style="color:blue;">k</span>', $input);
    $input = preg_replace ('#<_k */>#',   '<span style="color:blue;">k</span>', $input);

    // Handle <_l/> within $input
    $input = preg_replace ('#<l */>#',    '<span style="color:blue;">l</span>', $input);
    $input = preg_replace ('#<_l */>#',   '<span style="color:blue;">l</span>', $input);

    // Handle <_lc/> within $input
    $input = preg_replace ('#<_lc */>#',  '<span style="color:blue;">&#x27A5;</span>', $input);

    // Handle <_ij/> within $input
    $input = preg_replace ('#<ij */>#',   '<span style="color:blue;">ij</span>', $input);
    $input = preg_replace ('#<_ij */>#',  '<span style="color:blue;">ij</span>', $input);

    // Handle <_jk/> within $input
    $input = preg_replace ('#<jk */>#',   '<span style="color:blue;">jk</span>', $input);
    $input = preg_replace ('#<_jk */>#',  '<span style="color:blue;">jk</span>', $input);

    // Handle <_kl/> within $input
    $input = preg_replace ('#<kl */>#',   '<span style="color:blue;">kl</span>', $input);
    $input = preg_replace ('#<_kl */>#',  '<span style="color:blue;">kl</span>', $input);

    // Handle <_os/> within $input
    $input = preg_replace ('#<os */>#',   '<span style="color:blue;">os</span>', $input);
    $input = preg_replace ('#<_os */>#',  '<span style="color:blue;">os</span>', $input);

    // Handle <_oi/> within $input
    $input = preg_replace ('#<oi */>#',   '<span style="color:blue;">oi</span>', $input);
    $input = preg_replace ('#<_oi */>#',  '<span style="color:blue;">oi</span>', $input);

    // Handle <_oj/> within $input
    $input = preg_replace ('#<oj */>#',   '<span style="color:blue;">oj</span>', $input);
    $input = preg_replace ('#<_oj */>#',  '<span style="color:blue;">oj</span>', $input);

    // Handle <_ok/> within $input
    $input = preg_replace ('#<ok */>#',   '<span style="color:blue;">ok</span>', $input);
    $input = preg_replace ('#<_ok */>#',  '<span style="color:blue;">ok</span>', $input);

    // Handle <_p/> within $input
    $input = preg_replace ('#<pi */>#',   '<span style="color:red;">p</span>', $input);
    $input = preg_replace ('#<_p */>#',   '<span style="color:red;">p</span>', $input);

    // Handle <_pom/> within $input
    $input = preg_replace ('#<pom */>#',  '<span style="color:red;">&#x00B1;</span>', $input);
    $input = preg_replace ('#<_pom */>#', '<span style="color:red;">&#x00B1;</span>', $input);

    // Handle <_r/> within $input
    $input = preg_replace ('#<r */>#',    '<span style="color:red;">r</span>', $input);
    $input = preg_replace ('#<_r */>#',   '<span style="color:red;">r</span>', $input);

    // Handle <_s/> within $input
    $input = preg_replace ('#<s */>#',    '<span style="color:blue;">s</span>', $input);
    $input = preg_replace ('#<_s */>#',   '<span style="color:blue;">s</span>', $input);

    // Handle <_v/> within $input
    $input = preg_replace ('#<v */>#',    '<span style="color:red;">v</span>', $input);
    $input = preg_replace ('#<_v */>#',   '<span style="color:red;">v</span>', $input);

    // Handle <_x/> within $input
    $input = preg_replace ('#<x */>#',    '<span style="color:red;">x</span>', $input);
    $input = preg_replace ('#<_x */>#',   '<span style="color:red;">x</span>', $input);

    return '<span ' . $class . 'style="' . $style . '">' . $input . '</span>';
} // End renderAPLcom

// The callback function for marking a digit specially
function mark_digit ($input, array $args, Parser $parser, PPFrame $frame)
{
    global $md_style;

    return '<span style="' . $md_style . '">' . $input . '</span>';
} // End mark_digit

// The callback function for converting PN input text to HTML output
function pnNotation ($input, array $args, Parser $parser, PPFrame $frame)
{
    return '<span style="color:red;">' . $input . '</span>';
} // End pnNotation

// The callback function for converting LC input text to HTML output
function lcNotation ($input, array $args, Parser $parser, PPFrame $frame)
{
    return '<span style="color:blue;">&#x27A5;</span>';
} // End lcNotation

// The callback function for converting HC input text to HTML output
function hcNotation ($input, array $args, Parser $parser, PPFrame $frame)
{
    return '<span style="color:blue;">' . $input . '</span>';
} // End hcNotation

// The callback function for converting <ad/> input text to HTML output
function pn_ad_Notation ($input, array $args, Parser $parser, PPFrame $frame)
{
    return hcNotation ('ad', $args, $parser, $frame);
} // End pn_ad_Notation

// The callback function for converting <_ah/> input text to HTML output
function pn_ah_Notation ($input, array $args, Parser $parser, PPFrame $frame)
{
    return hcNotation ('ah', $args, $parser, $frame);
} // End pn_ah_Notation

// The callback function for converting <ar/> input text to HTML output
function pn_ar_Notation ($input, array $args, Parser $parser, PPFrame $frame)
{
    return hcNotation ('ar', $args, $parser, $frame);
} // End pn_ar_Notation

// The callback function for converting <au/> input text to HTML output
function pn_au_Notation ($input, array $args, Parser $parser, PPFrame $frame)
{
    return hcNotation ('au', $args, $parser, $frame);
} // End pn_au_Notation

// The callback function for converting <bn/> input text to HTML output
function pn_bn_Notation ($input, array $args, Parser $parser, PPFrame $frame)
{
    return pnNotation ('b', $args, $parser, $frame);
} // End pn_bn_Notation

// The callback function for converting <e/> input text to HTML output
function e1Notation ($input, array $args, Parser $parser, PPFrame $frame)
{
    return pnNotation ('e', $args, $parser, $frame);
} // End e1Notation

// The callback function for converting <E/> input text to HTML output
function E2Notation ($input, array $args, Parser $parser, PPFrame $frame)
{
    return pnNotation ('E', $args, $parser, $frame);
} // End E2Notation

// The callback function for converting <g/> input text to HTML output
function pn_g_Notation ($input, array $args, Parser $parser, PPFrame $frame)
{
    return pnNotation ('g', $args, $parser, $frame);
} // End pn_g_Notation

// The callback function for converting <in/> input text to HTML output
function pn_in_Notation ($input, array $args, Parser $parser, PPFrame $frame)
{
    return hcNotation ('i', $args, $parser, $frame);
} // End pn_in_Notation

// The callback function for converting <ij/> input text to HTML output
function pn_ij_Notation ($input, array $args, Parser $parser, PPFrame $frame)
{
    return hcNotation ('ij', $args, $parser, $frame);
} // End pn_ij_Notation

// The callback function for converting <jk/> input text to HTML output
function pn_jk_Notation ($input, array $args, Parser $parser, PPFrame $frame)
{
    return hcNotation ('jk', $args, $parser, $frame);
} // End pn_jk_Notation

// The callback function for converting <kl/> input text to HTML output
function pn_kl_Notation ($input, array $args, Parser $parser, PPFrame $frame)
{
    return hcNotation ('kl', $args, $parser, $frame);
} // End pn_kl_Notation

// The callback function for converting <j/> input text to HTML output
function j1Notation ($input, array $args, Parser $parser, PPFrame $frame)
{
    return hcNotation ('j', $args, $parser, $frame);
} // End j1Notation

// The callback function for converting <J/> input text to HTML output
function J2Notation ($input, array $args, Parser $parser, PPFrame $frame)
{
    return hcNotation ('J', $args, $parser, $frame);
} // End J2Notation

// The callback function for converting <k/> input text to HTML output
function pn_k_Notation ($input, array $args, Parser $parser, PPFrame $frame)
{
    return hcNotation ('k', $args, $parser, $frame);
} // End pn_k_Notation

// The callback function for converting <l/> input text to HTML output
function pn_l_Notation ($input, array $args, Parser $parser, PPFrame $frame)
{
    return hcNotation ('l', $args, $parser, $frame);
} // End pn_l_Notation

// The callback function for converting <_lc/> input text to HTML output
function pn_lc_Notation ($input, array $args, Parser $parser, PPFrame $frame)
{
    return lcNotation ('lc', $args, $parser, $frame);
} // End pn_lc_Notation

// The callback function for converting <pi/> input text to HTML output
function pn_pi_Notation ($input, array $args, Parser $parser, PPFrame $frame)
{
    return pnNotation ('p', $args, $parser, $frame);
} // End pn_pi_Notation

// The callback function for converting <os/> input text to HTML output
function pn_os_Notation ($input, array $args, Parser $parser, PPFrame $frame)
{
    return hcNotation ('os', $args, $parser, $frame);
} // End pn_os_Notation

// The callback function for converting <oi/> input text to HTML output
function pn_oi_Notation ($input, array $args, Parser $parser, PPFrame $frame)
{
    return hcNotation ('oi', $args, $parser, $frame);
} // End pn_oi_Notation

// The callback function for converting <oj/> input text to HTML output
function pn_oj_Notation ($input, array $args, Parser $parser, PPFrame $frame)
{
    return hcNotation ('oj', $args, $parser, $frame);
} // End pn_oj_Notation

// The callback function for converting <ok/> input text to HTML output
function pn_ok_Notation ($input, array $args, Parser $parser, PPFrame $frame)
{
    return hcNotation ('ok', $args, $parser, $frame);
} // End pn_ok_Notation

// The callback function for converting <pom/> input text to HTML output
function pn_pom_Notation ($input, array $args, Parser $parser, PPFrame $frame)
{
    return pnNotation ('&#x00B1;', $args, $parser, $frame);
} // End pn_pom_Notation

// The callback function for converting <r/> input text to HTML output
function pn_r_Notation ($input, array $args, Parser $parser, PPFrame $frame)
{
    return pnNotation ('r', $args, $parser, $frame);
} // End pn_r_Notation

// The callback function for converting <s/> input text to HTML output
function pn_s_Notation ($input, array $args, Parser $parser, PPFrame $frame)
{
    return pnNotation ('s', $args, $parser, $frame);
} // End pn_s_Notation

// The callback function for converting <v/> input text to HTML output
function pn_v_Notation ($input, array $args, Parser $parser, PPFrame $frame)
{
    return pnNotation ('v', $args, $parser, $frame);
} // End pn_v_Notation

// The callback function for converting <x/> input text to HTML output
function pn_x_Notation ($input, array $args, Parser $parser, PPFrame $frame)
{
    return pnNotation ('x', $args, $parser, $frame);
} // End pn_x_Notation


?>
