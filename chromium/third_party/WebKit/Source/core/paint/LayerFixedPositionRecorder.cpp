// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "core/paint/LayerFixedPositionRecorder.h"

#include "core/layout/LayoutBoxModelObject.h"
#include "platform/RuntimeEnabledFeatures.h"
#include "platform/graphics/GraphicsContext.h"
#include "platform/graphics/paint/FixedPositionContainerDisplayItem.h"
#include "platform/graphics/paint/FixedPositionDisplayItem.h"
#include "platform/graphics/paint/PaintController.h"

namespace blink {

LayerFixedPositionRecorder::LayerFixedPositionRecorder(GraphicsContext& graphicsContext, const LayoutBoxModelObject& layoutObject)
    : m_graphicsContext(graphicsContext)
    , m_layoutObject(layoutObject)
    , m_isFixedPosition(layoutObject.style()->position() == FixedPosition)
    , m_isFixedPositionContainer(layoutObject.canContainFixedPositionObjects())
{
    if (!RuntimeEnabledFeatures::slimmingPaintV2Enabled())
        return;

    if (m_isFixedPosition)
        m_graphicsContext.paintController().createAndAppend<BeginFixedPositionDisplayItem>(m_layoutObject);

    // TODO(trchen): Adding a pair of display items on every transformed
    // element can be expensive. Investigate whether we can optimize out some
    // of them if applicable.
    if (m_isFixedPositionContainer)
        m_graphicsContext.paintController().createAndAppend<BeginFixedPositionContainerDisplayItem>(m_layoutObject);
}

LayerFixedPositionRecorder::~LayerFixedPositionRecorder()
{
    if (!RuntimeEnabledFeatures::slimmingPaintV2Enabled())
        return;

    if (m_isFixedPositionContainer)
        m_graphicsContext.paintController().endItem<EndFixedPositionDisplayItem>(m_layoutObject);
    if (m_isFixedPosition)
        m_graphicsContext.paintController().endItem<EndFixedPositionDisplayItem>(m_layoutObject);
}

} // namespace blink