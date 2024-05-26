/*
 * Copyright (C) 2022 l-m.dev
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include "demos.h"
#include <string.h>
#include <time.h>
#include <stdlib.h>

typedef struct {
	const char *name;
	int score;
} entry_t;

enum {
	SORT_ALPHABETICAL = 0,
	SORT_SCORE = 1,
};

enum {
	SEARCH_LINEAR = 0,
	SEARCH_BINARY = 1,
};

static struct {
	int i;
	int pivot;
} bubble_sort_data;

static struct {
	int left_bound;
	int right_bound;
} binary_search_data;

static struct {
	int i;
} linear_search_data;

enum {
	STATE_NONE,
	STATE_SORT,
	STATE_SEARCH,
};

static int state_kind = STATE_NONE;
static int sort_kind = SORT_SCORE;
static int search_kind = SEARCH_LINEAR;
static int search_target_score;

// 21 for null sentinel
static int entries_len = 4;
static entry_t name_entries[21] = {
	{ "bamer", 50 },
	{ "aamer", 20 },
	{ "camer", 80 },
	{ "gaamer", 10 },
};

void bubble_sort_swap(entry_t *entries, int a, int b) {
	entry_t temp = entries[a];
	entries[a] = entries[b];
	entries[b] = temp;
}

bool is_sorted_by_score() {
	for (int i = 0; i < entries_len - 1; i++) {
		if (name_entries[i].score > name_entries[i + 1].score) {
			return false;
		}
	}

	return true;
}

void part_a_window(void) {
	igBegin("Part A", 0, ImGuiWindowFlags_AlwaysAutoResize);

	bool add_entry = false;
	static char name_input_buf[128] = {};
	static char score_input_buf[128] = {};

	igBeginDisabled(state_kind != STATE_NONE); // disable if we are sorting or searching
	if (igBeginTable("Entry", 3, 0, V2ZERO, 20.0f)) {
		// Move to the next row in the table
		igTableNextRow(0, 0);

		igTableNextColumn();
		{
			igInputTextEx("Enter Names", "", name_input_buf, sizeof(name_input_buf), (ImVec2){100.f, 0.f}, 0, 0, 0);
		}

		igTableNextColumn();
		{
			igInputTextEx("Enter Score", "", score_input_buf, sizeof(score_input_buf), (ImVec2){100.f, 0.f}, ImGuiInputTextFlags_CharsDecimal, 0, 0);
		}

		igTableNextColumn();
		{
			add_entry = igButton("Add Entry", V2ZERO);
		}

		igEndTable();
	}

	if (add_entry && entries_len < 20) {
		int score = strtol(score_input_buf, NULL, 10);

		name_entries[entries_len] = (entry_t){
			.name = strdup(name_input_buf),
			.score = score,
		};
		entries_len++;
	}

	igSeparator();

	if (igBeginTable("Sort", 2, 0, V2ZERO, 20.0f)) {
		igTableNextRow(0, 0);

		igTableNextColumn();
		{
			static const char* combo_items[] = {
				"Alphabetical",
				"Score",
			};

			igCombo_Str_arr("Sort Type",&sort_kind, combo_items, 2, 0);
		}

		igTableNextColumn();
		{
			if (igButton("Go Sort!", V2ZERO)) {
				bubble_sort_data.i = 1;
				bubble_sort_data.pivot = entries_len;
				state_kind = STATE_SORT;
			}
		}

		igEndTable();
	}

	if (igBeginTable("Search", 2, 0, V2ZERO, 20.0f)) {
		igTableNextRow(0, 0);

		igTableNextColumn();
		{
			static const char* combo_items[] = {
				"Linear Search",
				"Binary Search",
			};

			igCombo_Str_arr("Search Type",&search_kind, combo_items, 2, 0);
		}

		igTableNextColumn();
		{
			igInputInt("Target Score", &search_target_score, 0, 0, 0);
		}

		igTableNextColumn();
		{
			// check if the array is sorted first
			const char *label = "Go Search!";
			bool search_invalid = search_kind == SEARCH_BINARY && !is_sorted_by_score();
			if (search_invalid) {
				label = "Array must be sorted by score.";
			}

			igBeginDisabled(search_invalid);
			if (igButton(label, V2ZERO)) {
				state_kind = STATE_SEARCH;
				if (search_kind == SEARCH_LINEAR) {
					linear_search_data.i = 0;
				} else {
					binary_search_data.left_bound = 0;
					binary_search_data.right_bound = entries_len - 1;
				}
			}
			igEndDisabled();
		}

		igEndTable();
	}
	igEndDisabled();

	igSeparator();

	if (igBeginTable("Array Display", 2, 0, V2ZERO, 20.0f)) {
		for (int i = 0; i < entries_len; i++) {
			igTableNextRow(0, 0);

			igTableNextColumn();
			{
				igText("%d", i);
			}

			igTableNextColumn();
			{
				igText("%s (score: %u)", name_entries[i].name, name_entries[i].score);
			}
		}

		igEndTable();
	}

	igEnd();
}

void part_a_search_window() {
	igBegin("Search", 0, ImGuiWindowFlags_AlwaysAutoResize);

	bool do_next = false;

	bool complete = false;
	int found_index = -1;

	int mid = (binary_search_data.left_bound + binary_search_data.right_bound) / 2;

	if (search_kind == SEARCH_LINEAR) {
		if (linear_search_data.i >= entries_len) {
			complete = true;
			
		} else if (name_entries[linear_search_data.i].score == search_target_score) {
			found_index = linear_search_data.i;
			complete = true;
		}
	} else {
		if (binary_search_data.left_bound > binary_search_data.right_bound) {
			complete = true;
		} else if (name_entries[mid].score == search_target_score) {
			found_index = mid;
			complete = true;
		}
	}

	bool exit = false;

	if (igBeginTable("Search", 2, 0, V2ZERO, 20.0f)) {
		igTableNextRow(0, 0);

		igBeginDisabled(complete);
		igTableNextColumn();
		{
			do_next = igButton("Next Step", V2ZERO);
		}
		if (search_kind == SEARCH_BINARY) {
			igTableNextColumn();
			{
				igText("Left: %d, Right: %d, Mid: %d", binary_search_data.left_bound, binary_search_data.right_bound, mid);
			}
		} else {
			igTableNextColumn();
			{
				igText("Search Index: %d (name: %s, score: %u)", linear_search_data.i, name_entries[linear_search_data.i].name, name_entries[linear_search_data.i].score);
			}
		}
		igEndDisabled();

		if (complete) {
			if (found_index == -1) {
				igSeparator();
				igText("Not found.");
			} else {
				igSeparator();
				igText("Found at index %d. (name: %s, score %u)", found_index, name_entries[found_index].name, name_entries[found_index].score);
			}

			if (igButton("Exit", V2ZERO)) {
				exit = true;
			}
		}

		igEndTable();
	}

	if (do_next) {
		if (search_kind == SEARCH_LINEAR) {
			linear_search_data.i++;
		} else {
			if (name_entries[mid].score < search_target_score) {
				binary_search_data.left_bound = mid + 1;
			} else {
				binary_search_data.right_bound = mid - 1;
			}
		}
	}

	igEnd();

	if (exit) {
		state_kind = STATE_NONE;
	}
}

void part_a_sort_window() {
	// bubble sort complete
	if (bubble_sort_data.pivot == 0) {
		state_kind = STATE_NONE;
		return;
	}

	igBegin("Sort", 0, ImGuiWindowFlags_AlwaysAutoResize);

	bool do_next = false;
	
	if (igBeginTable("Sort", 2, 0, V2ZERO, 20.0f)) {
		igTableNextRow(0, 0);

		igTableNextColumn();
		{
			do_next = igButton("Next Step", V2ZERO);
		}

		igEndTable();
	}

	if (do_next) {
		// perform 1 iteration, at most one swap
		if (bubble_sort_data.i >= bubble_sort_data.pivot) {
			bubble_sort_data.i = 1;
			bubble_sort_data.pivot--;	
		}

		if (sort_kind == SORT_SCORE) {
			if (name_entries[bubble_sort_data.i - 1].score > name_entries[bubble_sort_data.i].score) {
				bubble_sort_swap(name_entries, bubble_sort_data.i - 1, bubble_sort_data.i);
			}
		} else {
			if (strcmp(name_entries[bubble_sort_data.i - 1].name, name_entries[bubble_sort_data.i].name) > 0) {
				bubble_sort_swap(name_entries, bubble_sort_data.i - 1, bubble_sort_data.i);
			}
		}

		bubble_sort_data.i++;
	}

	igEnd();

	if (state_kind == STATE_SORT) {
		part_a_sort_window();
	}
	if (state_kind == STATE_SEARCH) {
		part_a_search_window();
	}
}

void insert_string_into_array(char *dest, const char *src, int dest_len, int insert_len, int cursor_location) {
	for (int i = dest_len; i >= cursor_location; i--) {
		dest[i + insert_len] = dest[i];
	}

	for (int i = 0; i < insert_len; i++) {
		dest[cursor_location + i] = src[i];
	}
}

void part_b_window() {
	igBegin("Part B", 0, ImGuiWindowFlags_AlwaysAutoResize);

	static char initial_sentence[256];
	int initial_sentence_len = strlen(initial_sentence);

	enum {
		STATE_DISPLAY,
		STATE_INSERT,
	};

	static int state = STATE_DISPLAY;
	static int cursor_location = 0;

	// text display
	{
		char *display_text = initial_sentence;
		
		if (state == STATE_INSERT) {
			static char edit_sentence[256];
			
			// splice in a cursor at the cursor location
			strncpy(edit_sentence, initial_sentence, cursor_location);
			edit_sentence[cursor_location] = '|';
			strcpy(edit_sentence + cursor_location + 1, initial_sentence + cursor_location);

			display_text = edit_sentence;
		}
		
		igBeginDisabled(state != STATE_DISPLAY);
		igInputTextEx("##InitialSentence", "Enter Initial Sentence", display_text, sizeof(initial_sentence), V2ZERO, 0, 0, 0);
		igEndDisabled();

		igSameLine(0, 0);

		if (state == STATE_DISPLAY) {
			if (igButton("Edit", V2ZERO)) {
				state = STATE_INSERT;
			}
		} else {
			if (igButton("Save", V2ZERO)) {
				state = STATE_DISPLAY;
			}
		}
	}

	igSeparator();

	igBeginDisabled(state == STATE_DISPLAY);
	{
		// left button
		if (igButton("Left", V2ZERO)) {
			if (cursor_location > 0) {
				cursor_location--;
			}
		}

		igSameLine(0, 8.f);

		// right button
		if (igButton("Right", V2ZERO)) {
			if (cursor_location < initial_sentence_len) {
				cursor_location++;
			}
		}

		igSameLine(0, 8.f);

		igText("Cursor Location: %d", cursor_location);

		static char insert_string[128];

		igInputTextEx("##InsertSentence", "Insert Sentence", insert_string, sizeof(insert_string), V2ZERO, 0, 0, 0);

		if (igButton("Insert", V2ZERO)) {
			// insert the string at the cursor location
			int insert_len = strlen(insert_string);
			if (insert_len + initial_sentence_len >= (int)sizeof(initial_sentence)) {
				// too long
				return;
			}

			// shift everything to the right
			for (int i = initial_sentence_len; i >= cursor_location; i--) {
				initial_sentence[i + insert_len] = initial_sentence[i];
			}

			// insert the string
			insert_string_into_array(initial_sentence, insert_string, initial_sentence_len, insert_len, cursor_location);

			// move the cursor
			cursor_location += insert_len;
		}
	}
	igEndDisabled();

	igEnd();
}

void frame(void) {
	FRAME_PASS_BEGIN;

	part_a_window();
	part_b_window();

	BLIT_BG(IM_COL32(50, 50, 50, 255));

	FRAME_PASS_END;
}