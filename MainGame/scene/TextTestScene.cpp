//
// Copyright (c) 2016-2018 João Baptista de Paula e Silva.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//


#include "TextTestScene.hpp"
#include <assert.hpp>

#include "ColorList.hpp"
#include "rendering/Renderer.hpp"

#include <chronoUtils.hpp>
#include "chronoUtils.hpp"

const std::string DemoStrings[] =
{
    u8"Hi. I am the text rendering engine being built for the game \"Rebound\", made in C++. "
    u8"I have some unique features, like being able to render RTL text and mixed text in-between, "
    u8"and being able to move and animate each word and each grapheme of the string individually. "
    u8"It's a very hard work, but it's very rewarding.",
    
    u8"こんにちは。 私はC++で作られたゲーム\"Rebound\"用に構築されているテキストレンダリングエンジンです。"
    u8"私は、右から左へのテキストとその間に混在したテキストをレンダリングすることができ、"
    u8"文字列の各単語と各書体を個別に移動したり動かすことができるという独自の機能を持っています。"
    u8"それは非常に難しい作業ですが、それはとても有益です。申し訳ありませんが、"
    u8"私の作者はテストするためにこのテキストを機械翻訳しなければなりません。",
    
    u8"مرحبا. أنا محرك تقديم النص يجري بناؤها للعبة \"Rebound\"\u200B"
    u8"، المحرز في C++\u200B. لدي بعض الميزات الفريدة، مثل القدرة على تقديم النص من اليمين إلى اليسار والن"
    u8"ص المختلط في ما بين، والقدرة على نقل وتحريك كل كلمة و كل غرافيم من سلسلة بشكل فردي. إنه عمل شاق جدا، ولكنه"
    u8" مجزي جدا. أنا آسف، ولكن مؤلفي لديه آلة ترجمة هذا النص من أجل أن يكون شيئا لاختبار.",
    
    u8"היי. אני מנוע עיבוד טקסט הנבנה עבור המשחק \"Rebound\"\u200D,"
    u8" שנעשו C++\u200D. יש לי כמה תכונות ייחודיות, כמו להיות מסוגל לעבד מימין לשמאל טקסט מעורב טקסט בין לבין, להיות"
    u8" מסוגל להעביר הנפש כל מילה וכל grapheme של המחרוזת בנפרד. זו עבודה קשה מאוד, אבל זה מאוד מתגמל. אני מצטער, אבל"
    u8" המחבר שלי צריך מכונה לתרגם את הטקסט הזה כדי שיהיה משהו לבדוק."

};

const std::string Fonts[] =
{
    "RobotoMono-Regular.ttf",
    "umeplus-gothic.ttf",
    "LateefRegOT.ttf",
    "FrankRuhlLibre-Regular.ttf"
};

const bool isRTL[] = { false, false, true, true };

constexpr size_t NumDemos = sizeof(DemoStrings)/sizeof(DemoStrings[0]);

using namespace std::literals::chrono_literals;

TextTestScene::TextTestScene(ResourceManager &manager) : resourceManager(manager), 
    testText(manager.load<FontHandler>("RobotoMono-Regular.ttf")), curDemo(0)
{
    testText.setFontSize(36);
    testText.setWordWrappingWidth(736);
    testText.setOutlineThickness(1);
    testText.setDefaultOutlineColor(Colors::Amber);
    
    setupDemo();
}

void TextTestScene::setupDemo()
{
    testText.setString(DemoStrings[curDemo]);
    testText.setFontHandler(resourceManager.load<FontHandler>(Fonts[curDemo]));
    testText.setRTL(isRTL[curDemo]);
    
    if (curDemo == 1) testText.setWordWrappingAlgorithm(TextDrawable::WordWrappingAlgorithm::Japanese);
    else testText.setWordWrappingAlgorithm(TextDrawable::WordWrappingAlgorithm::Normal);
    
    testText.buildGeometry();
    
    for (size_t j = 0; j < testText.getNumberOfGraphemeClusters(); j++)
    {
        for (auto& v : testText.getGraphemeCluster(j)) v.color.a = 0;
        for (auto& v : testText.getGraphemeCluster(j, true)) v.color.a = 0;
    }
}

void TextTestScene::update(FrameTime curTime)
{
    if (initTime == decltype(initTime)()) initTime = curTime;
    
    auto duration = curTime - initTime;
    float curStep = toSeconds<float>(duration) / 0.125;
    
    for (size_t i = 0; i < testText.getNumberOfGraphemeClusters(); i++)
    {
        for (auto& v : testText.getGraphemeCluster(i))
        {
            if (curStep < i) v.color.a = 0;
            else if (curStep >= i+1) v.color.a = 255;
            else v.color.a = 255 * (curStep - i);
        }
        for (auto& v : testText.getGraphemeCluster(i, true))
        {
            if (curStep < i) v.color.a = 0;
            else if (curStep >= i+1) v.color.a = 255;
            else v.color.a = 255 * (curStep - i);
        }
    }
    
    if (curDemo < NumDemos-1 && curStep >= testText.getNumberOfGraphemeClusters() + 12)
    {
        float curFade = (curStep - (testText.getNumberOfGraphemeClusters() + 12)) / 8;
        
        for (size_t i = 0; i < testText.getNumberOfGraphemeClusters(); i++)
        {
            for (auto& v : testText.getGraphemeCluster(i)) v.color.a = 255 * (1.0f - curFade);
            for (auto& v : testText.getGraphemeCluster(i, true)) v.color.a = 255 * (1.0f - curFade);
        }
    }
    
    if (curDemo < NumDemos-1 && curStep >= testText.getNumberOfGraphemeClusters() + 20)
    {
        curDemo++;
        setupDemo();
        initTime = curTime;
    }
}

void TextTestScene::render(Renderer &renderer)
{
    renderer.pushTransform();
    renderer.currentTransform *= util::translate(16, 16);
    renderer.pushDrawable(testText, 20);
    renderer.popTransform();
}
