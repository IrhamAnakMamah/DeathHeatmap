#include <Geode/Geode.hpp>
#include <Geode/modify/PlayerObject.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>
#include <map>
#include <vector>
#include <algorithm>

using namespace geode::prelude;

std::map<int, std::vector<float>> g_sessionDeaths;

void addDeathLog(int levelID, float percent) {
    g_sessionDeaths[levelID].push_back(percent);
}

std::vector<float> getDeathLog(int levelID) {
    if (g_sessionDeaths.contains(levelID)) {
        return g_sessionDeaths[levelID];
    }
    return {};
}


class HeatmapPopup : public FLAlertLayer {
public:
    int m_levelID;
    CCScale9Sprite* m_barHitbox = nullptr;
    CCLabelBMFont* m_detailsLabel = nullptr;
    std::map<int, int> m_deathCounts;
    int m_maxDeathsInOneSpot = 1; 

    void update(float dt) override {
        if (!m_barHitbox || !m_detailsLabel) return;

        auto mousePos = cocos::getMousePos();
        CCPoint posInBar = m_barHitbox->convertToNodeSpace(mousePos);
        CCSize barSize = m_barHitbox->getContentSize();

        bool isHovering = (posInBar.x >= 0 && posInBar.x <= barSize.width &&
                           posInBar.y >= 0 && posInBar.y <= barSize.height);

        if (isHovering) {
            float ratio = posInBar.x / barSize.width;
            int percent = static_cast<int>(ratio * 100.0f);

            if (percent < 0) percent = 0;
            if (percent > 100) percent = 100;

            int count = m_deathCounts[percent];

            if (count > 0) {
                m_detailsLabel->setString(fmt::format("Position: {}%  |  Deaths: {}", percent, count).c_str());
                m_detailsLabel->setColor({255, 100, 100}); 
            } else {
                m_detailsLabel->setString(fmt::format("Position: {}%  |  Safe", percent).c_str());
                m_detailsLabel->setColor({100, 255, 255}); 
            }
            m_detailsLabel->setOpacity(255);
        } else {
            m_detailsLabel->setString("Hover over the bar to see details");
            m_detailsLabel->setColor({150, 150, 150});
            m_detailsLabel->setOpacity(150);
        }
    }

    ccColor4F getColorForIntensity(int count) {
        if (count == 0) {
            return {0.0f, 0.8f, 1.0f, 0.6f}; 
        }

        float ratio = (float)count / (float)m_maxDeathsInOneSpot;
        float r = ratio;                
        float g = 1.0f - ratio;         
        float b = 1.0f - ratio;         
        float alpha = 0.6f + (ratio * 0.4f); 

        return {r, g, b, alpha};
    }

    void drawRectHelper(CCDrawNode* drawNode, CCPoint bottomLeft, CCPoint topRight, ccColor4F color) {
        CCPoint verts[4] = {
            {bottomLeft.x, bottomLeft.y},   // Kiri Bawah
            {topRight.x, bottomLeft.y},     // Kanan Bawah
            {topRight.x, topRight.y},       // Kanan Atas
            {bottomLeft.x, topRight.y}      // Kiri Atas
        };
        drawNode->drawPolygon(verts, 4, color, 0, {0, 0, 0, 0});
    }

    bool init(int levelID) {
        if (!FLAlertLayer::init(nullptr, "Session Heatmap", "", "OK", nullptr, 380.f, false, 240.f, 1.0f)) {
            return false;
        }

        m_levelID = levelID;
        auto winSize = CCDirector::sharedDirector()->getWinSize();

        std::vector<float> rawDeaths = getDeathLog(m_levelID);
        m_maxDeathsInOneSpot = 1; 

        for (float p : rawDeaths) {
            int pInt = static_cast<int>(p);
            if (pInt < 0) pInt = 0;
            if (pInt > 100) pInt = 100;
            m_deathCounts[pInt]++;
            
            if (m_deathCounts[pInt] > m_maxDeathsInOneSpot) {
                m_maxDeathsInOneSpot = m_deathCounts[pInt];
            }
        }

        auto countLabel = CCLabelBMFont::create(
            fmt::format("Total Attempts: {}", rawDeaths.size()).c_str(), 
            "goldFont.fnt"
        );
        countLabel->setScale(0.7f);
        countLabel->setPosition({ winSize.width / 2, winSize.height / 2 + 65 });
        m_mainLayer->addChild(countLabel);

        float barWidth = 320.0f;
        float barHeight = 25.0f; 
        float barX = (winSize.width / 2) - (barWidth / 2);
        float barY = (winSize.height / 2); 

        m_barHitbox = CCScale9Sprite::create("square02_small.png");
        m_barHitbox->setContentSize({ barWidth, barHeight });
        m_barHitbox->setOpacity(0); 
        m_barHitbox->setAnchorPoint({ 0, 0 }); 
        m_barHitbox->setPosition({ barX, barY });
        m_mainLayer->addChild(m_barHitbox, 100);

        auto drawNode = CCDrawNode::create();
        
        CCPoint bgBottomLeft = { barX, barY };
        CCPoint bgTopRight = { barX + barWidth, barY + barHeight };
        
        drawRectHelper(drawNode, bgBottomLeft, bgTopRight, {0.0f, 0.0f, 0.0f, 0.5f});

        float segWidth = barWidth / 100.0f;

        for (int i = 0; i < 100; i++) {
            int deathCount = m_deathCounts[i];
            ccColor4F color = getColorForIntensity(deathCount);

            float startX = barX + (i * segWidth);
            float endX = startX + segWidth;
            
            CCPoint segBottom = { startX, barY };
            CCPoint segTop = { endX, barY + barHeight };

            drawRectHelper(drawNode, segBottom, segTop, color);
        }
        
        m_mainLayer->addChild(drawNode, 90); 

        m_detailsLabel = CCLabelBMFont::create("Hover over the bar to see details", "chatFont.fnt");
        m_detailsLabel->setPosition({ winSize.width / 2, barY - 20 });
        m_detailsLabel->setScale(0.6f);
        m_detailsLabel->setColor({150, 150, 150});
        m_mainLayer->addChild(m_detailsLabel, 105);

        auto startLabel = CCLabelBMFont::create("0%", "chatFont.fnt");
        startLabel->setPosition({ barX - 20, barY + (barHeight/2) });
        startLabel->setScale(0.4f);
        startLabel->setColor({200, 200, 200});
        m_mainLayer->addChild(startLabel);

        auto endLabel = CCLabelBMFont::create("100%", "chatFont.fnt");
        endLabel->setPosition({ barX + barWidth + 25, barY + (barHeight/2) });
        endLabel->setScale(0.4f);
        endLabel->setColor({200, 200, 200});
        m_mainLayer->addChild(endLabel);

        this->scheduleUpdate();

        return true;
    }

    static HeatmapPopup* create(int levelID) {
        HeatmapPopup* ret = new HeatmapPopup();
        if (ret && ret->init(levelID)) {
            ret->autorelease();
            return ret;
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }
};

class $modify(MyDeathTracker, PlayerObject) {
    void playerDestroyed(bool p0) {
        PlayerObject::playerDestroyed(p0);

        auto pl = PlayLayer::get();
        if (!pl) return;
        if (this != pl->m_player1) return;
        if (pl->m_isPracticeMode) return;

        float percentage = pl->getCurrentPercent();
        int levelID = pl->m_level->m_levelID.value();

        addDeathLog(levelID, percentage);
    }
};

class $modify(MyHeatmapBtn, LevelInfoLayer) {
    void onOpenHeatmap(CCObject*) {
        int levelID = this->m_level->m_levelID.value();
        HeatmapPopup::create(levelID)->show();
    }

    bool init(GJGameLevel* level, bool challenge) {
        if (!LevelInfoLayer::init(level, challenge)) return false;

        auto spr = CCSprite::createWithSpriteFrameName("GJ_statsBtn_001.png");
        spr->setScale(0.8f);

        auto btn = CCMenuItemSpriteExtra::create(
            spr,
            this,
            menu_selector(MyHeatmapBtn::onOpenHeatmap)
        );
        btn->setID("heatmap-button"_spr); 

        auto menu = this->getChildByID("left-side-menu");
        if (!menu) {
            menu = CCMenu::create();
            menu->setID("heatmap-menu");
            menu->setPosition({ 30, 200 }); 
            this->addChild(menu);
        }

        menu->addChild(btn);
        menu->updateLayout();

        return true;
    }
};