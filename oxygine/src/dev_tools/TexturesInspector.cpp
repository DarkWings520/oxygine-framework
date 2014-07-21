#include "TexturesInspector.h"
#include "core/NativeTexture.h"
#include "AnimationFrame.h"
#include "Sprite.h"
#include "initActor.h"
#include "SlidingActor.h"
#include "DebugActor.h"
#include "res/Resources.h"
#include "TextField.h"
#include "ColorRectSprite.h"
#include "utils/stringUtils.h"

namespace oxygine
{
	Vector2 fitSize(const Vector2 &destSize, const Vector2 &src);
	const Vector2 itemSize(128.0f, 128.0f);

	class TextureLine: public Box9Sprite
	{
	public:
		TextureLine(spNativeTexture t)
		{
			setVerticalMode(Box9Sprite::TILING_FULL);
			setHorizontalMode(Box9Sprite::TILING_FULL);
			setResAnim(DebugActor::resSystem->getResAnim("checker"));

			AnimationFrame f;
			Vector2 s = fitSize(itemSize, Vector2((float)t->getWidth(), (float)t->getHeight()));

			setSize(s);

			Diffuse df;
			df.base = t;

			f.init(0, df, RectF(0, 0, 1.0f, 1.0f), RectF(0, 0, s.x, s.y), s);
			spSprite image = initActor(new Sprite,
				arg_blend = blend_disabled,
				arg_resAnim = f);
			addChild(image);

			spColorRectSprite rect = initActor(new ColorRectSprite,
				arg_color = Color(Color::White, 128),
				arg_attachTo = this);

			char path[255];
			path::normalize(t->getName().c_str(), path);

			char txt[255];
			safe_sprintf(txt, "%s\n<div c=\"ff0000\">%s</div>-<div c=\"0000ff\">%dx%d</div>\nid:%d", 
				path,
				textureFormat2String(t->getFormat()),
				t->getWidth(), t->getHeight(), t->getObjectID());

			spTextField text = initActor(new TextField,
				arg_color = Color::Black,
				arg_w = (float)itemSize.x,
				arg_vAlign = TextStyle::VALIGN_TOP,
				arg_hAlign = TextStyle::HALIGN_LEFT,
				arg_multiline = true,
				arg_attachTo = rect,
				arg_htmlText = txt
				);


			rect->setSize(text->getTextRect().size.cast<Vector2>() + Vector2(2, 2));
			rect->setY((itemSize.y - rect->getHeight())/2.0f);
		}

	};

	TexturesInspector::TexturesInspector(const Vector2 &size)
	{
		setSize(size);

		spSlidingActor slide = new SlidingActor;
		slide->setSize(size);
		addChild(slide);


		float offsetY = 0;

		vector<spNativeTexture> textures = NativeTexture::getCreatedTextures();

		
		spTextField text = initActor(new TextField,
			arg_color = Color::White,
			arg_w = itemSize.x * 3.0f,
			arg_h = 30.0f,
			arg_vAlign = TextStyle::VALIGN_TOP,
			arg_hAlign = TextStyle::HALIGN_LEFT,
			arg_multiline = true,
			arg_attachTo = slide
			);

		offsetY += text->getTextRect().getBottom() + 5;

		spActor content = new Actor;
		
		
		int numX = (int)(size.x / itemSize.x);

		float y = 0;
		int n = 0;
		int mem = 0;
		for (vector<spNativeTexture>::iterator i = textures.begin(); i != textures.end(); ++i)
		{
			spNativeTexture t = *i;
			TextureLine *line = new TextureLine(t);

			float x = (n % numX) * (itemSize.x + 5.0f);
			float y = (n / numX) * (itemSize.y + 5.0f);
			line->setX(x);
			line->setY(y + offsetY);
			content->addChild(line);
			++n;

			mem += t->getSizeVRAM();
		}

		char txt[255];
		safe_sprintf(txt, "created textures: %d, vram: %d kb", textures.size(), mem / 1024);

		text->setText(txt);
		if (numX > n)
			numX = n;

		content->setSize(
			(itemSize.x + 5.0f) * numX, 
			(itemSize.y + 5.0f) * (n + numX - 1.0f) / numX + offsetY);

		setWidth(content->getWidth());
		slide->setWidth(content->getWidth());

		//slide->setSize()
		slide->setContent(content);
	}

	TexturesInspector::~TexturesInspector()
	{

	}
}