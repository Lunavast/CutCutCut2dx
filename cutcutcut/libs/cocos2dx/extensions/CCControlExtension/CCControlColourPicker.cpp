/*
 * CCControlColourPicker.m
 *
 * Copyright 2012 Stewart Hamilton-Arrandale.
 * http://creativewax.co.uk
 *
 * Modified by Yannick Loriot.
 * http://yannickloriot.com
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
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * converted to c++ / cocos2d-x by Angus C
 */

#include "CCControlColourPicker.h"
#include "CCPointExtension.h"
#include "CCSpriteFrameCache.h"
#include "CCSpriteBatchNode.h"

NS_CC_EXT_BEGIN

bool CCControlColourPicker::init()
{
    if (CCControl::init())
    {
        setIsTouchEnabled(true);
        // Cache the sprites
        CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("extensions/CCControlColourPickerSpriteSheet.plist");
        
        // Create the sprite batch node
        CCSpriteBatchNode *spriteSheet  = CCSpriteBatchNode::batchNodeWithFile("extensions/CCControlColourPickerSpriteSheet.png");
        addChild(spriteSheet);
        
        // MIPMAP
        ccTexParams params  = {GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT};
        spriteSheet->getTexture()->setAliasTexParameters();
        spriteSheet->getTexture()->setTexParameters(&params);
        spriteSheet->getTexture()->generateMipmap();

        // Init default color
        m_hsv.h = 0;
        m_hsv.s = 0;
        m_hsv.v = 0;
        
        // Add image
        m_background=CCControlUtils::addSpriteToTargetWithPosAndAnchor("menuColourPanelBackground.png", spriteSheet, CCPointZero, ccp(0.5f, 0.5f));
       
        CCPoint backgroundPointZero = ccpSub(m_background->getPosition(), ccp (m_background->getContentSize().width / 2, m_background->getContentSize().height / 2));
        
        // Setup panels . currently hard-coded...
        float hueShift                = 8;
        float colourShift             = 28;
        
        m_huePicker=CCControlHuePicker::pickerWithTargetAndPos(spriteSheet, ccp(backgroundPointZero.x + hueShift, backgroundPointZero.y + hueShift));
        m_colourPicker=CCControlSaturationBrightnessPicker::pickerWithTargetAndPos(spriteSheet, ccp(backgroundPointZero.x + colourShift, backgroundPointZero.y + colourShift));
        
        // Setup events
        m_huePicker->addTargetWithActionForControlEvents(this, menu_selector(CCControlColourPicker::hueSliderValueChanged), CCControlEventValueChanged);
        m_colourPicker->addTargetWithActionForControlEvents(this, menu_selector(CCControlColourPicker::colourSliderValueChanged), CCControlEventValueChanged);
       
        // Set defaults
        updateHueAndControlPicker();
        addChild(m_huePicker);
        addChild(m_colourPicker);

        // Set content size
        setContentSize(m_background->getContentSize());
        return true;
    }
    else
        return false;
}

CCControlColourPicker* CCControlColourPicker::colourPicker()
{
    CCControlColourPicker *pRet = new CCControlColourPicker();
    pRet->init();
    pRet->autorelease();
    return pRet;
}


void CCControlColourPicker::setColorValue(const ccColor3B& colorValue)
{
    m_colorValue      = colorValue;
    
    RGBA rgba;
    rgba.r      = colorValue.r / 255.0f;
    rgba.g      = colorValue.g / 255.0f;
    rgba.b      = colorValue.b / 255.0f;
    rgba.a      = 1.0f;
    
    m_hsv=CCControlUtils::HSVfromRGB(rgba);
    updateHueAndControlPicker();
}

//need two events to prevent an infinite loop! (can't update huePicker when the huePicker triggers the callback due to CCControlEventValueChanged)
void CCControlColourPicker::updateControlPicker()
{
    m_huePicker->setHue(m_hsv.h);
    m_colourPicker->updateWithHSV(m_hsv);
}

void CCControlColourPicker::updateHueAndControlPicker()
{
    m_huePicker->setHue(m_hsv.h);
    m_colourPicker->updateWithHSV(m_hsv);
    m_colourPicker->updateDraggerWithHSV(m_hsv);
}


void CCControlColourPicker::hueSliderValueChanged(CCObject * sender)
{
    m_hsv.h      = ((CCControlHuePicker*)sender)->getHue();

    // Update the value
    RGBA rgb    = CCControlUtils::RGBfromHSV(m_hsv);
    m_colorValue= ccc3((GLubyte)(rgb.r * 255.0f), (GLubyte)(rgb.g * 255.0f), (GLubyte)(rgb.b * 255.0f));
    
    // Send CCControl callback
    sendActionsForControlEvents(CCControlEventValueChanged);
    updateControlPicker();
}

void CCControlColourPicker::colourSliderValueChanged(CCObject * sender)
{
    m_hsv.s=((CCControlSaturationBrightnessPicker*)sender)->getSaturation();
    m_hsv.v=((CCControlSaturationBrightnessPicker*)sender)->getBrightness();


     // Update the value
    RGBA rgb    = CCControlUtils::RGBfromHSV(m_hsv);
    m_colorValue=ccc3((GLubyte)(rgb.r * 255.0f), (GLubyte)(rgb.g * 255.0f), (GLubyte)(rgb.b * 255.0f));
    
    // Send CCControl callback
    sendActionsForControlEvents(CCControlEventValueChanged);
}

//ignore all touches, handled by children
bool CCControlColourPicker::ccTouchBegan(CCTouch* touch, CCEvent* pEvent)
{
    return false;
}

NS_CC_EXT_END
