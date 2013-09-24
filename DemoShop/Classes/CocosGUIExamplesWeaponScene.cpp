

#include "CocosGUIExamplesWeaponScene.h"


const char* shop_textures[8] =
{
    "cocosgui/gui_examples/DemoShop/armour.png",
    "cocosgui/gui_examples/DemoShop/helmet.png",
    "cocosgui/gui_examples/DemoShop/shield.png",
    "cocosgui/gui_examples/DemoShop/sword.png",
    "cocosgui/gui_examples/DemoShop/gloves.png",
    "cocosgui/gui_examples/DemoShop/dimensity.png",
    "cocosgui/gui_examples/DemoShop/dart.png",
    "cocosgui/gui_examples/DemoShop/backpack.png",
};

const char* shop_names[8] =
{
    "Armour",
    "Helmet",
    "Shield",
    "Sword",
    "Gloves",
    "Dimensity",
    "Dart",
    "Backpack",
};

const char* shop_price_units[8] =
{
    "Counpon",
    "Binding",
    "Medal",
    "Counpon",
    "Binding",
    "Medal",
    "Counpon",
    "Binding",
};

const int shop_prices[8] =
{
    19,
    10,
    22,
    20,
    8,
    17,
    5,
    4,
};

CocosGUIExamplesWeaponScene::CocosGUIExamplesWeaponScene()
: m_nIndex(-1)
, m_nCount(0)
, m_nCoupon(COUPON_MAX)
, m_nBinding(BINDING_MAX)
, m_nMedal(MEDAL_MAX)
{
    CCScene::init();
}

CocosGUIExamplesWeaponScene::~CocosGUIExamplesWeaponScene()
{
    
}

void CocosGUIExamplesWeaponScene::onEnter()
{
    CCScene::onEnter();
    
    m_pUILayer = UILayer::create();
    m_pUILayer->scheduleUpdate();
    addChild(m_pUILayer);
    
    ShopInit();
    popupInit();
//    BuyInit();    
}

void CocosGUIExamplesWeaponScene::onExit()
{
    m_pUILayer->removeFromParent();
    
    CCSSceneReader::sharedSceneReader()->purgeSceneReader();
    UIHelper::purgeUIHelper();
	cocos2d::extension::ActionManager::shareManager()->purgeActionManager();
    
    CCScene::onExit();
}

void CocosGUIExamplesWeaponScene::menuCloseCallback(CCObject* pSender, TouchEventType type)
{
    if (type == TOUCH_EVENT_ENDED)
    {
        CCDirector::sharedDirector()->end();
        
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
        exit(0);
#endif
    }
}

// shop
void CocosGUIExamplesWeaponScene::ShopInit()
{
    // shop Layout from json
    Layout* shop_root = static_cast<Layout*>(CCUIHELPER->createWidgetFromJsonFile("cocosgui/gui_examples/DemoShop/DemoShop.json"));
    m_pUILayer->addWidget(shop_root);    
    
    // shop scrollview
    UIScrollView* shop_scrollview = static_cast<UIScrollView*>(shop_root->getChildByName("shop_ScrollView"));
    // shop scrollview children
    for (int i = 0; i < shop_scrollview->getChildren()->count(); ++i)
    {
        Layout* shop_layout = static_cast<Layout*>(shop_scrollview->getChildren()->objectAtIndex(i));
        shop_layout->setTag(SHOP_ITEM_LAYOUT_TAG + i);
        
        // buy button
        UIButton* buy_button = static_cast<UIButton*>(shop_layout->getChildByName("buy_Button"));
        buy_button->addTouchEventListener(this, toucheventselector(CocosGUIExamplesWeaponScene::popupLogic));
    }
    
    // ranking scrollview
    UIScrollView* ranking_scrollview = static_cast<UIScrollView*>(shop_root->getChildByName("ranking_ScrollView"));
    // ranking scrollview children
    for (int i = 0; i < ranking_scrollview->getChildren()->count(); ++i)
    {
        Layout* ranking_layout = static_cast<Layout*>(ranking_scrollview->getChildren()->objectAtIndex(i));
        
        for (int j = 0; j < shop_scrollview->getChildren()->count(); ++j)
        {
            Layout* shop_layout = static_cast<Layout*>(shop_scrollview->getChildren()->objectAtIndex(j));
            if (strcmp(ranking_layout->getName(), shop_layout->getName()) == 0)
            {
                ranking_layout->setTag(RANKING_ITEM_LAYOUT_TAG + (shop_layout->getTag() - SHOP_ITEM_LAYOUT_TAG));
            }
        }
    }
    for (int i = 0; i < ranking_scrollview->getChildren()->count(); ++i)
    {
        Layout* ranking_layout = static_cast<Layout*>(ranking_scrollview->getChildren()->objectAtIndex(i));
        
        // buy button
        UIButton* buy_button = static_cast<UIButton*>(ranking_layout->getChildByName("buy_Button"));
        buy_button->addTouchEventListener(this, toucheventselector(CocosGUIExamplesWeaponScene::popupLogic));
    }
    
    // back button
    UIButton* back_button = static_cast<UIButton*>(shop_root->getChildByName("back_Button"));
    back_button->addTouchEventListener(this, toucheventselector(CocosGUIExamplesWeaponScene::menuCloseCallback));
}

// popup
void CocosGUIExamplesWeaponScene::popupInit()
{
    // buy layout
    Layout* buy_layout = static_cast<Layout*>(m_pUILayer->getWidgetByName("buy_Panel"));
    
    // add button
    UIButton* add_button = static_cast<UIButton*>(buy_layout->getChildByName("add_Button"));
    add_button->addTouchEventListener(this, toucheventselector(CocosGUIExamplesWeaponScene::popupCalculate));
    
    // sub button
    UIButton* sub_button = static_cast<UIButton*>(buy_layout->getChildByName("sub_Button"));
    sub_button->addTouchEventListener(this, toucheventselector(CocosGUIExamplesWeaponScene::popupCalculate));
    
    // number labelatlas
    UILabelAtlas* number_labelAtlas = static_cast<UILabelAtlas*>(buy_layout->getChildByName("number_LabelAtlas"));
    number_labelAtlas->setStringValue(CCString::createWithFormat("%d", m_nCount)->getCString());
    
    // coupon number labelatlas
    UILabelAtlas* couponNumber_labelAtlas = static_cast<UILabelAtlas*>(buy_layout->getChildByName("coupon_number_LabelAtlas"));
    couponNumber_labelAtlas->setStringValue(CCString::createWithFormat("%d", m_nCoupon)->getCString());
    
    // binding number labelatlas
    UILabelAtlas* bindingNumber_labelAtlas = static_cast<UILabelAtlas*>(buy_layout->getChildByName("binding_number_LabelAtlas"));
    bindingNumber_labelAtlas->setStringValue(CCString::createWithFormat("%d", m_nBinding)->getCString());
    
    // medal number labelatlas
    UILabelAtlas* medalNumber_labelAtlas = static_cast<UILabelAtlas*>(buy_layout->getChildByName("medal_number_LabelAtlas"));
    medalNumber_labelAtlas->setStringValue(CCString::createWithFormat("%d", m_nMedal)->getCString());
    
    // buy button
    UIButton* buy_button = static_cast<UIButton*>(buy_layout->getChildByName("buy_Button"));
    buy_button->addTouchEventListener(this, toucheventselector(CocosGUIExamplesWeaponScene::popupClose));
    buy_button->setTouchEnabled(true);
    
    // close button
    UIButton* close_button = static_cast<UIButton*>(buy_layout->getChildByName("close_Button"));
    close_button->addTouchEventListener(this, toucheventselector(CocosGUIExamplesWeaponScene::popupClose));
}

void CocosGUIExamplesWeaponScene::popupClose(CCObject *pSender, TouchEventType type)
{
    if (type == TOUCH_EVENT_ENDED)
    {
        Layout* buy_layout = static_cast<Layout*>(m_pUILayer->getWidgetByName("buy_Panel"));
        buy_layout->setVisible(false);
        
        // process shop ranking touchEnabled
        CCObject* obj = NULL;
        
        // shop scrollview
        UIScrollView* shop_scrollview = static_cast<UIScrollView*>(m_pUILayer->getWidgetByName("shop_ScrollView"));
        // shop scrollview children
        CCARRAY_FOREACH(shop_scrollview->getChildren(), obj)
        {
            Layout* shop_layout = static_cast<Layout*>(obj);
            
            // buy button
            UIButton* buy_button = static_cast<UIButton*>(shop_layout->getChildByName("buy_Button"));
            buy_button->setTouchEnabled(true);
        }
        
        // ranking scrollview
        UIScrollView* ranking_scrollview = static_cast<UIScrollView*>(m_pUILayer->getWidgetByName("ranking_ScrollView"));
        // ranking scrollview children
        CCARRAY_FOREACH(ranking_scrollview->getChildren(), obj)
        {
            Layout* ranking_layout = static_cast<Layout*>(obj);
            
            // buy button
            UIButton* buy_button = static_cast<UIButton*>(ranking_layout->getChildByName("buy_Button"));
            buy_button->setTouchEnabled(true);
        }
    }
}

void CocosGUIExamplesWeaponScene::popupLogic(CCObject *pSender, TouchEventType type)
{
    if (type == TOUCH_EVENT_ENDED)
    {        
        UIWidget* widget = static_cast<UIWidget*>(pSender);
        UIWidget* parent = widget->getParent();
        
        // buy layout
        Layout* buy_layout = static_cast<Layout*>(m_pUILayer->getWidgetByName("buy_Panel"));
        buy_layout->setVisible(true);
        
        // icon imageview
        UIImageView* icon_imageview = static_cast<UIImageView*>(buy_layout->getChildByName("icon_ImageView"));
        // name labelBMFont
        UILabelBMFont* name_labelBMFont = static_cast<UILabelBMFont*>(buy_layout->getChildByName("name_LabelBMFont"));
        // price unit labelBMFont
        UILabelBMFont* priceUnit_labelBMFont = static_cast<UILabelBMFont*>(buy_layout->getChildByName("price_unit_LabelBMFont"));
        // price labelBMFont
        UILabelBMFont* price_labelBMFont = static_cast<UILabelBMFont*>(buy_layout->getChildByName("price_LabelBMFont"));
        
        UIScrollView* shop_scrollview = static_cast<UIScrollView*>(m_pUILayer->getWidgetByName("shop_ScrollView"));
        UIScrollView* ranking_scrollview = static_cast<UIScrollView*>(m_pUILayer->getWidgetByName("ranking_ScrollView"));
        int tag = parent->getTag();
        int index = 0;
        if (tag >= SHOP_ITEM_LAYOUT_TAG && tag <= shop_scrollview->getChildren()->count() + SHOP_ITEM_LAYOUT_TAG)
        {
            index = tag - SHOP_ITEM_LAYOUT_TAG;
        }
        else if (tag >= RANKING_ITEM_LAYOUT_TAG && tag <= ranking_scrollview->getChildren()->count() + RANKING_ITEM_LAYOUT_TAG)
        {
            index = tag - RANKING_ITEM_LAYOUT_TAG;
        }
        m_nIndex = index;
        icon_imageview->loadTexture(shop_textures[index]);
        name_labelBMFont->setText(shop_names[index]);
        priceUnit_labelBMFont->setText(shop_price_units[index]);
        price_labelBMFont->setText(CCString::createWithFormat("%d", shop_prices[index])->getCString());
        
        
        // reset buy property
        m_nCount = 0;
        // number labelatlas
        UILabelAtlas* number_labelAtlas = static_cast<UILabelAtlas*>(buy_layout->getChildByName("number_LabelAtlas"));
        number_labelAtlas->setStringValue(CCString::createWithFormat("%d", m_nCount)->getCString());
        
        m_nCoupon = COUPON_MAX;
        m_nBinding = BINDING_MAX;
        m_nMedal = MEDAL_MAX;
        // coupon number labelatlas
        UILabelAtlas* couponNumber_labelAtlas = static_cast<UILabelAtlas*>(buy_layout->getChildByName("coupon_number_LabelAtlas"));
        couponNumber_labelAtlas->setStringValue(CCString::createWithFormat("%d", m_nCoupon)->getCString());
        
        // binding number labelatlas
        UILabelAtlas* bindingNumber_labelAtlas = static_cast<UILabelAtlas*>(buy_layout->getChildByName("binding_number_LabelAtlas"));
        bindingNumber_labelAtlas->setStringValue(CCString::createWithFormat("%d", m_nBinding)->getCString());
        
        // medal number labelatlas
        UILabelAtlas* medalNumber_labelAtlas = static_cast<UILabelAtlas*>(buy_layout->getChildByName("medal_number_LabelAtlas"));
        medalNumber_labelAtlas->setStringValue(CCString::createWithFormat("%d", m_nMedal)->getCString());
        
        
        // process shop ranking touchEnabled
        CCObject* obj = NULL;
        
        // shop scrollview children
        CCARRAY_FOREACH(shop_scrollview->getChildren(), obj)
        {
            Layout* shop_layout = static_cast<Layout*>(obj);
            
            // buy button
            UIButton* buy_button = static_cast<UIButton*>(shop_layout->getChildByName("buy_Button"));
            buy_button->setTouchEnabled(false);
        }
        
        // ranking scrollview children
        CCARRAY_FOREACH(ranking_scrollview->getChildren(), obj)
        {
            Layout* ranking_layout = static_cast<Layout*>(obj);
            
            // buy button
            UIButton* buy_button = static_cast<UIButton*>(ranking_layout->getChildByName("buy_Button"));
            buy_button->setTouchEnabled(false);
        }
    }
}

void CocosGUIExamplesWeaponScene::popupCalculate(CCObject *pSender, TouchEventType type)
{
    if (type == TOUCH_EVENT_ENDED)
    {
        UIButton* button = static_cast<UIButton*>(pSender);
        UIWidget* buy_layout = button->getParent();
        
        int price = shop_prices[m_nIndex];
        if (strcmp(button->getName(), "add_Button") == 0)   // add
        {
            if (strcmp(shop_price_units[m_nIndex], "Counpon") == 0)
            {
                if (m_nCoupon >= price)
                {
                    m_nCount++;
                    m_nCoupon -= price;
                }                
            }
            else if (strcmp(shop_price_units[m_nIndex], "Binding") == 0)
            {
                if (m_nBinding >= price)
                {
                    m_nCount++;
                    m_nBinding -= price;
                }
            }
            if (strcmp(shop_price_units[m_nIndex], "Medal") == 0)
            {
                if (m_nMedal >= price)
                {
                    m_nCount++;
                    m_nMedal -= price;
                }
            }
        }
        else if (strcmp(button->getName(), "sub_Button") == 0)  // sub
        {
            if (m_nCount > 0)
            {
                m_nCount--;
                
                if (strcmp(shop_price_units[m_nIndex], "Counpon") == 0)
                {
                    m_nCoupon += price;
                }
                else if (strcmp(shop_price_units[m_nIndex], "Binding") == 0)
                {
                    m_nBinding += price;
                }
                if (strcmp(shop_price_units[m_nIndex], "Medal") == 0)
                {
                    m_nMedal += price;
                }
            }
        }
        
        // number labelatlas
        UILabelAtlas* number_labelAtlas = static_cast<UILabelAtlas*>(buy_layout->getChildByName("number_LabelAtlas"));
        number_labelAtlas->setStringValue(CCString::createWithFormat("%d", m_nCount)->getCString());
        
        // coupon number labelatlas
        UILabelAtlas* couponNumber_labelAtlas = static_cast<UILabelAtlas*>(buy_layout->getChildByName("coupon_number_LabelAtlas"));
        couponNumber_labelAtlas->setStringValue(CCString::createWithFormat("%d", m_nCoupon)->getCString());
        
        // binding number labelatlas
        UILabelAtlas* bindingNumber_labelAtlas = static_cast<UILabelAtlas*>(buy_layout->getChildByName("binding_number_LabelAtlas"));
        bindingNumber_labelAtlas->setStringValue(CCString::createWithFormat("%d", m_nBinding)->getCString());
        
        // medal number labelatlas
        UILabelAtlas* medalNumber_labelAtlas = static_cast<UILabelAtlas*>(buy_layout->getChildByName("medal_number_LabelAtlas"));
        medalNumber_labelAtlas->setStringValue(CCString::createWithFormat("%d", m_nMedal)->getCString());
    }
}
