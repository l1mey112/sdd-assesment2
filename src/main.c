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
#define USE_INIT2
#include "demos.h"
#include <ctype.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

void init2(void) {
	ImFont *font = ImFontAtlas_AddFontFromFileTTF(igGetIO()->Fonts, "TerminusTTF-4.49.3.ttf", 26, NULL, NULL);

	
}

typedef struct {
	const char *name;
	int score;
} entry_t;

enum {
	SORT_ALPHABETICAL,
	SORT_SCORE,
};

struct {
	int i;
	int pivot;
	int sort_kind;
	int entries_len;
	entry_t name_entries[20];
} bubble_sort_data;

static bool do_sort = false;
static int sort_kind = SORT_SCORE;

void bubble_sort_swap(entry_t *entries, int a, int b) {
	entry_t temp = entries[a];
	entries[a] = entries[b];
	entries[b] = temp;
}

void main_window(void) {
    igBegin("Main", 0, ImGuiWindowFlags_AlwaysAutoResize);

	bool add_entry = false;
	static char name_input_buf[128] = {};
	static char score_input_buf[128] = {};

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

	static int entries_len = 4;
	static entry_t name_entries[20] = {
		{ "bamer", 50 },
		{ "aamer 1", 20 },
		{ "camer 5", 80 },
		{ "gaamer 8", 10 },
	};

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
				bubble_sort_data.sort_kind = sort_kind;
				bubble_sort_data.entries_len = entries_len;
				memcpy(bubble_sort_data.name_entries, name_entries, sizeof(name_entries));
				do_sort = true;
			}
		}

		igEndTable();
	}
	
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
				igText("%s (%u)", name_entries[i].name, name_entries[i].score);
			}
		}

		igEndTable();
	}

    igEnd();
}

void sort_window() {
	if (bubble_sort_data.pivot == 0) {
		do_sort = false;
		return;
	}
	
    igBegin("Sort Viewer", 0, ImGuiWindowFlags_AlwaysAutoResize);

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

		if (bubble_sort_data.sort_kind == SORT_SCORE) {
			if (bubble_sort_data.name_entries[bubble_sort_data.i - 1].score > bubble_sort_data.name_entries[bubble_sort_data.i].score) {
				bubble_sort_swap(bubble_sort_data.name_entries, bubble_sort_data.i - 1, bubble_sort_data.i);
			}
		} else {
			if (strcmp(bubble_sort_data.name_entries[bubble_sort_data.i - 1].name, bubble_sort_data.name_entries[bubble_sort_data.i].name) > 0) {
				bubble_sort_swap(bubble_sort_data.name_entries, bubble_sort_data.i - 1, bubble_sort_data.i);
			}
		}

		bubble_sort_data.i++;
	}

	igSeparator();

	// Display the sorted array
	if (igBeginTable("Sorted Array", 2, ImGuiTableFlags_Borders, V2ZERO, 20.0f)) {
		for (int i = 0; i < bubble_sort_data.entries_len; i++) {
			igTableNextRow(0, 0);

			igTableNextColumn();
			{
				igText("%d", i);
			}

			igTableNextColumn();
			{
				igText("%s (%u)", bubble_sort_data.name_entries[i].name, bubble_sort_data.name_entries[i].score);
			}
		}

		igEndTable();
	}

    igEnd();
}

void frame(void) {
	FRAME_PASS_BEGIN;

	main_window();
	if (do_sort) {
		sort_window();
	}

	BLIT_BG(IM_COL32(50, 50, 50, 255));

	FRAME_PASS_END;
}