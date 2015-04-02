// Aseprite
// Copyright (C) 2001-2015  David Capello
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License version 2 as
// published by the Free Software Foundation.

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "app/cmd/replace_image.h"

#include "doc/image.h"
#include "doc/image_io.h"
#include "doc/image_ref.h"
#include "doc/sprite.h"
#include "doc/subobjects_io.h"

namespace app {
namespace cmd {

using namespace doc;

ReplaceImage::ReplaceImage(Sprite* sprite, const ImageRef& oldImage, const ImageRef& newImage)
  : WithSprite(sprite)
  , m_oldImageId(oldImage->id())
  , m_newImageId(newImage->id())
  , m_newImage(newImage)
{
}

void ReplaceImage::onExecute()
{
  // Save old image in m_copy. We cannot keep an ImageRef to this
  // image, because there are other undo branches that could try to
  // modify/re-add this same image ID
  ImageRef oldImage = sprite()->getImageRef(m_oldImageId);
  ASSERT(oldImage);
  m_copy.reset(Image::createCopy(oldImage.get()));

  sprite()->replaceImage(m_oldImageId, m_newImage);
  m_newImage.reset();
}

void ReplaceImage::onUndo()
{
  ImageRef newImage = sprite()->getImageRef(m_newImageId);
  ASSERT(newImage);
  ASSERT(!sprite()->getImageRef(m_oldImageId));
  m_copy->setId(m_oldImageId);

  sprite()->replaceImage(m_newImageId, m_copy);
  m_copy.reset(Image::createCopy(newImage.get()));
}

void ReplaceImage::onRedo()
{
  ImageRef oldImage = sprite()->getImageRef(m_oldImageId);
  ASSERT(oldImage);
  ASSERT(!sprite()->getImageRef(m_newImageId));
  m_copy->setId(m_newImageId);

  sprite()->replaceImage(m_oldImageId, m_copy);
  m_copy.reset(Image::createCopy(oldImage.get()));
}

} // namespace cmd
} // namespace app
