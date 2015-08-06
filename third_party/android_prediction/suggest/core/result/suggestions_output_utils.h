/*
 * Copyright (C) 2013 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef LATINIME_SUGGESTIONS_OUTPUT_UTILS
#define LATINIME_SUGGESTIONS_OUTPUT_UTILS

#include "third_party/android_prediction/defines.h"

namespace latinime {

class BinaryDictionaryShortcutIterator;
class DicNode;
class DicTraverseSession;
class Scoring;
class SuggestionResults;

class SuggestionsOutputUtils {
 public:
    /**
     * Outputs the final list of suggestions (i.e., terminal nodes).
     */
    static void outputSuggestions(const Scoring *const scoringPolicy,
            DicTraverseSession *traverseSession, const float languageWeight,
            SuggestionResults *const outSuggestionResults);

 private:
    DISALLOW_IMPLICIT_CONSTRUCTORS(SuggestionsOutputUtils);

    // Inputs longer than this will autocorrect if the suggestion is multi-word
    static const int MIN_LEN_FOR_MULTI_WORD_AUTOCORRECT;

    static void outputSuggestionsOfDicNode(const Scoring *const scoringPolicy,
            DicTraverseSession *traverseSession, const DicNode *const terminalDicNode,
            const float languageWeight, const bool boostExactMatches,
            const bool forceCommitMultiWords, const bool outputSecondWordFirstLetterInputIndex,
            SuggestionResults *const outSuggestionResults);
    static void outputShortcuts(BinaryDictionaryShortcutIterator *const shortcutIt,
            const int finalScore, const bool sameAsTyped,
            SuggestionResults *const outSuggestionResults);
    static int computeFirstWordConfidence(const DicNode *const terminalDicNode);
};
} // namespace latinime
#endif // LATINIME_SUGGESTIONS_OUTPUT_UTILS
