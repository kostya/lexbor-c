/*
 * Copyright (C) 2018 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#include "lexbor/html/tree/insertion_mode.h"
#include "lexbor/html/tree/open_elements.h"


static bool
lxb_html_tree_insertion_mode_in_table_body_tr(lxb_html_tree_t *tree,
                                              lxb_html_token_t *token);

static bool
lxb_html_tree_insertion_mode_in_table_body_thtd(lxb_html_tree_t *tree,
                                                lxb_html_token_t *token);

static bool
lxb_html_tree_insertion_mode_in_table_body_tbtfth_closed(lxb_html_tree_t *tree,
                                                         lxb_html_token_t *token);

static bool
lxb_html_tree_insertion_mode_in_table_body_ct_open_closed(lxb_html_tree_t *tree,
                                                          lxb_html_token_t *token);

static bool
lxb_html_tree_insertion_mode_in_table_body_bcht_closed(lxb_html_tree_t *tree,
                                                       lxb_html_token_t *token);

static bool
lxb_html_tree_insertion_mode_in_table_body_anything_else(lxb_html_tree_t *tree,
                                                         lxb_html_token_t *token);

static bool
lxb_html_tree_insertion_mode_in_table_body_anything_else_closed(lxb_html_tree_t *tree,
                                                                lxb_html_token_t *token);

static void
lxb_html_tree_clear_stack_back_to_table_body(lxb_html_tree_t *tree);


#include "lexbor/html/tree/insertion_mode/in_table_body_res.h"


bool
lxb_html_tree_insertion_mode_in_table_body(lxb_html_tree_t *tree,
                                           lxb_html_token_t *token)
{
    if (token->tag_id >= LXB_TAG__LAST_ENTRY) {
        if (token->type & LXB_HTML_TOKEN_TYPE_CLOSE) {
            return lxb_html_tree_insertion_mode_in_table_body_anything_else_closed(tree,
                                                                                   token);
        }

        return lxb_html_tree_insertion_mode_in_table_body_anything_else(tree, token);
    }

    if (token->type & LXB_HTML_TOKEN_TYPE_CLOSE) {
        return lxb_html_tree_insertion_mode_in_table_body_closed_res[token->tag_id](tree,
                                                                                    token);
    }

    return lxb_html_tree_insertion_mode_in_table_body_res[token->tag_id](tree,
                                                                         token);
}

static bool
lxb_html_tree_insertion_mode_in_table_body_tr(lxb_html_tree_t *tree,
                                              lxb_html_token_t *token)
{
    lxb_html_element_t *element;

    lxb_html_tree_clear_stack_back_to_table_body(tree);

    element = lxb_html_tree_insert_html_element(tree, token);
    if (element == NULL) {
        tree->status = LXB_STATUS_ERROR_MEMORY_ALLOCATION;

        return lxb_html_tree_process_abort(tree);
    }

    tree->mode = lxb_html_tree_insertion_mode_in_row;

    return true;
}

/*
 * "th", "td"
 */
static bool
lxb_html_tree_insertion_mode_in_table_body_thtd(lxb_html_tree_t *tree,
                                                lxb_html_token_t *token)
{
    lxb_html_token_t fake_token;
    lxb_html_element_t *element;

    lxb_html_tree_parse_error(tree, token, LXB_HTML_RULES_ERROR_UNTO);

    lxb_html_tree_clear_stack_back_to_table_body(tree);

    fake_token = *token;

    fake_token.tag_id = LXB_TAG_TR;
    fake_token.attr_first = NULL;
    fake_token.attr_last = NULL;

    element = lxb_html_tree_insert_html_element(tree, &fake_token);
    if (element == NULL) {
        tree->status = LXB_STATUS_ERROR_MEMORY_ALLOCATION;

        return lxb_html_tree_process_abort(tree);
    }

    tree->mode = lxb_html_tree_insertion_mode_in_row;

    return false;
}

/*
 * "tbody", "tfoot", "thead"
 */
static bool
lxb_html_tree_insertion_mode_in_table_body_tbtfth_closed(lxb_html_tree_t *tree,
                                                         lxb_html_token_t *token)
{
    lxb_dom_node_t *node;

    node = lxb_html_tree_element_in_scope(tree, token->tag_id, LXB_NS_HTML,
                                          LXB_HTML_TAG_CATEGORY_SCOPE_TABLE);
    if (node == NULL) {
        lxb_html_tree_parse_error(tree, token, LXB_HTML_RULES_ERROR_MIELINSC);

        return true;
    }

    lxb_html_tree_clear_stack_back_to_table_body(tree);
    lxb_html_tree_open_elements_pop(tree);

    tree->mode = lxb_html_tree_insertion_mode_in_table;

    return true;
}

/*
 * A start tag whose tag name is one of: "caption", "col", "colgroup", "tbody",
 * "tfoot", "thead"
 * An end tag whose tag name is "table"
 */
static bool
lxb_html_tree_insertion_mode_in_table_body_ct_open_closed(lxb_html_tree_t *tree,
                                                          lxb_html_token_t *token)
{
    lxb_dom_node_t *node;

    node = lxb_html_tree_element_in_scope_tbody_thead_tfoot(tree);
    if (node == NULL) {
        lxb_html_tree_parse_error(tree, token, LXB_HTML_RULES_ERROR_MIELINSC);

        return true;
    }

    lxb_html_tree_clear_stack_back_to_table_body(tree);
    lxb_html_tree_open_elements_pop(tree);

    tree->mode = lxb_html_tree_insertion_mode_in_table;

    return false;
}

/*
 * "body", "caption", "col", "colgroup", "html", "td", "th", "tr"
 */
static bool
lxb_html_tree_insertion_mode_in_table_body_bcht_closed(lxb_html_tree_t *tree,
                                                       lxb_html_token_t *token)
{
    lxb_html_tree_parse_error(tree, token, LXB_HTML_RULES_ERROR_UNCLTO);

    return true;
}

static bool
lxb_html_tree_insertion_mode_in_table_body_anything_else(lxb_html_tree_t *tree,
                                                         lxb_html_token_t *token)
{
    return lxb_html_tree_insertion_mode_in_table(tree, token);
}

static bool
lxb_html_tree_insertion_mode_in_table_body_anything_else_closed(lxb_html_tree_t *tree,
                                                                lxb_html_token_t *token)
{
    return lxb_html_tree_insertion_mode_in_table_body_anything_else(tree,
                                                                    token);
}

static void
lxb_html_tree_clear_stack_back_to_table_body(lxb_html_tree_t *tree)
{
    lxb_dom_node_t *current = lxb_html_tree_current_node(tree);

    while ((current->tag_id != LXB_TAG_TBODY
            && current->tag_id != LXB_TAG_TFOOT
            && current->tag_id != LXB_TAG_THEAD
            && current->tag_id != LXB_TAG_TEMPLATE
            && current->tag_id != LXB_TAG_HTML)
           || current->ns != LXB_NS_HTML)
    {
        lxb_html_tree_open_elements_pop(tree);
        current = lxb_html_tree_current_node(tree);
    }
}
