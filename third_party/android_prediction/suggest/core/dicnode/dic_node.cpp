/*
 * Copyright (C) 2012 The Android Open Source Project
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

#include "third_party/android_prediction/suggest/core/dicnode/dic_node.h"

namespace latinime {

DicNode::DicNode(const DicNode &dicNode)
        :
#if DEBUG_DICT
          mProfiler(dicNode.mProfiler),
#endif
          mDicNodeProperties(dicNode.mDicNodeProperties), mDicNodeState(dicNode.mDicNodeState),
          mIsCachedForNextSuggestion(dicNode.mIsCachedForNextSuggestion) {
    /* empty */
}

DicNode &DicNode::operator=(const DicNode &dicNode) {
#if DEBUG_DICT
    mProfiler = dicNode.mProfiler;
#endif
    mDicNodeProperties = dicNode.mDicNodeProperties;
    mDicNodeState = dicNode.mDicNodeState;
    mIsCachedForNextSuggestion = dicNode.mIsCachedForNextSuggestion;
    return *this;
}

} // namespace latinime