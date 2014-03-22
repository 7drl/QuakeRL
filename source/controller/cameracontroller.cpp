#include "cameracontroller.h"

const s32 kViewW = 1024;
const s32 kViewH = 614;
const f32 kHalfViewW = kViewW / 2.0f;
const f32 kHalfViewH = kViewH / 2.0f;

CameraController::CameraController()
	: pCamera(nullptr)
	, cOffset(-kHalfViewW, -kHalfViewH, 0.0f)
	, cArea(0.0f, 0.0f, 0.0f, 0.0f)
{
}

CameraController::~CameraController()
{
}

void CameraController::SetCamera(Camera *cam)
{
	pCamera = cam;
}

void CameraController::SetArea(const Rect4f &area)
{
	cArea = area;
}

void CameraController::LookAt(const Vector3f &pos)
{
	/*
	Texel/Raster bug - arrendodar a posicao da camera para pixel-perfect.
	*/
	auto p = pos + cOffset;
	auto x = static_cast<s32>(p.getX());
	auto y = static_cast<s32>(p.getY());
	auto z = p.getZ();

	auto halfScreenWidth = s32(kHalfViewW);//pScreen->GetWidth() / 2;
	auto halfScreenHeight = s32(kHalfViewH);//pScreen->GetHeight() / 2;

	if (x < s32(cArea.x1 - 16))
		x = s32(cArea.x1 - 16);

	if (y < s32(cArea.y1 - 16))
		y = s32(cArea.y1 - 16);

	if (y > s32(cArea.y2 - 16 - halfScreenHeight * 2))
		y = s32(cArea.y2 - 16 - halfScreenHeight * 2);

	if (x > s32(cArea.x2 - 16 - halfScreenWidth * 2))
		x = s32(cArea.x2 - 16 - halfScreenWidth * 2);

	auto np = Vector3f(static_cast<f32>(x), static_cast<f32>(y), z);
	pCamera->SetPosition(np);
}
