/*****************************************************************************/
/**
 *  @file   main.cpp
 *  @brief  Example program for Point/Line/Polygon/Glyph.
 *  @author Insight, Inc.
 */
#include <kvs/Version>
#include <kvs/PointObject>
#include <kvs/LineObject>
#include <kvs/PolygonObject>
#include <kvs/UnstructuredVolumeObject>
#include <kvs/HydrogenVolumeData>
#include <kvs/CellByCellMetropolisSampling>

#include <kvs/PointRenderer>
#include <kvs/LineRenderer>
#include <kvs/PolygonRenderer>
#include <kvs/ArrowGlyph>
#include <kvs/DiamondGlyph>
#include <kvs/SphereGlyph>

// #include <kvs/TornadoVolumeData>

#include <Application.h>
#include <Screen.h>
#include <TouchController.h>
// #include "ImGuiWidgetHandler.h"
// #include "ImGuiWindow.h"
// #include "ImGuiWidget.h"

namespace KVSMain {
	// 点の数
	const static size_t N = 4;

	// 線分の数
	const static size_t L = 6;

	// ポリゴンの数
	const static size_t P = 4;

	// 四面体の数
	const static size_t T = 1;

	// 座標値配列の定義
	//kvs::Real32 CoordArray[N * 3] = {
	//	1.0, 0.0, 2.0,  // 点0の座標 (1.0, 0.0, 2.0)
	//	2.0, 0.0, 2.0,  // 点1の座標 (2.0, 0.0, 2.0)
	//	1.5, 0.8, 1.7,  // 点2の座標 (1.5, 0.8, 1.7)
	//	1.5, 0.0, 1.1   // 点3の座標 (1.5, 0.0, 1.1)
	//};
	static kvs::Real32 CoordArray[N * 3] = {
		100,   0, 200,  // 点0の座標 (1.0, 0.0, 2.0)
		200,   0, 200,  // 点1の座標 (2.0, 0.0, 2.0)
		150,  80, 170,  // 点2の座標 (1.5, 0.8, 1.7)
		150,   0, 110   // 点3の座標 (1.5, 0.0, 1.1)
	};

	// カラー値配列の定義
	static kvs::UInt8 ColorArray[N * 3] = {
		255,   0,   0,  // 点0の色 赤(255,   0,   0)
		  0, 255,   0,  // 点1の色 緑(  0, 255,   0)
		  0,   0, 255,  // 点2の色 青(  0,   0, 255)
		255, 255,   0   // 点3の色 黄(255, 255,   0)
	};

	// 法線ベクトル配列の定義
	static kvs::Real32 NormalArray[N * 3] = {
		0.00,  0.3,  0.8,  // 点0に対する法線ベクトル
		0.00, -0.9,  0.0,  // 点1に対する法線ベクトル
		0.72,  0.3, -0.4,  // 点2に対する法線ベクトル
		-0.72,  0.3, -0.4   // 点3に対する法線ベクトル
	};

	// 線分の接続情報配列の定義
	static kvs::UInt32 LineConnectionArray[L * 2] = {
		0, 1,  // 点0と点1を接続
		1, 2,  // 点1と点2を接続
		2, 0,  // 点2と点0を接続
		3, 0,  // 点3と点0を接続
		3, 1,  // 点3と点1を接続
		3, 2   // 点3と点2を接続
	};

	// ポリゴンの接続情報配列の定義
	static kvs::UInt32 PolygonConnectionArray[P * 3] = {
		0, 1, 2,  // 点0, 1, 2を接続
		0, 3, 1,  // 点0, 3, 1を接続
		1, 3, 2,  // 点0, 3, 2を接続
		0, 2, 3   // 点0, 2, 3を接続
	};

	// arrow glyph用connections : CellType==tetrahedraとする
	static kvs::UInt32 GlyphConnectionArray[N] = {
		0, 1, 2, 3,
	};

	const static size_t D = 1;
	static kvs::Real32 GlyphValueArray[N * D] = {
		100.0f,
		200.0f,
		300.0f,
		400.0f,
	};

	static kvs::Real32 DiamondGlyphCoordArray[N * 3] = {
		90,  -10, 210,  // 点0の座標 (1.0, 0.0, 2.0)
		210, -10, 210,  // 点1の座標 (2.0, 0.0, 2.0)
		150,  90, 150,  // 点2の座標 (1.5, 0.8, 1.7)
		150, -10, 100   // 点3の座標 (1.5, 0.0, 1.1)
	};

	// Polyline
	const static size_t Ny = 8;
	static kvs::Real32 PolylineCoordArray[Ny * 3] = {
		  0, -200, 100,
		  0, -100, 100,
		100, -100, 100,
		100, -200, 100,
		  0, -200, 200,
		  0, -100, 200,
		100, -100, 200,
		100, -200, 200,
	};

	static kvs::UInt8 PolylineColorArray[Ny * 3] = {
		  0,   0,   0,
		  0, 255,   0,
		255, 255,   0,
		255,   0,   0,
		  0,   0, 255,
		  0, 255, 255,
		255, 255, 255,
		255,   0, 255,
	};

	const static size_t Ly = 2;
	static kvs::UInt32 PolylineConnectionArray[Ly * 2] = {
		0, 3,
		4, 7,
	};

	// Uniline
	const static size_t Nu = 8;
	static kvs::Real32 UnilineCoordArray[Nu * 3] = {
		 -10, -200, 100,
		 -10, -100, 100,
		-110, -100, 100,
		-110, -200, 100,
		 -10, -200, 200,
		 -10, -100, 200,
		-110, -100, 200,
		-110, -200, 200,
	};

	const static size_t Lu = 7;
	static kvs::UInt32 UnilineConnectionArray[Lu + 1] = {
		0,1,2,3,7,6,5,4
	};

	static kvs::UInt8 UnilineColorArray[Lu * 3] = {
		  0,   0,   0,
		  0, 255,   0,
		255, 255,   0,
		255,   0,   0,
		  0,   0, 255,
		  0, 255, 255,
		255, 255, 255,
	};

	const static size_t Ns = 10;
	static kvs::Real32 StripCoordArray[Ns * 3] = {
		-120, -200, 100,
		-120, -100, 100,
		-220, -100, 100,
		-220, -200, 100,
		-120, -200, 100,

		-120, -200, 200,
		-120, -100, 200,
		-220, -100, 200,
		-220, -200, 200,
		-120, -200, 200,
	};
	static kvs::UInt8 StripColorArray[(Ns - 1) * 3] = {
		  0,   0,   0,
		255,   0,   0,
		  0, 255,   0,
		  0,   0, 255,
		255, 255,   0,
		255,   0, 255,
		  0, 255, 255,
		255, 255, 255,
		  0,   0,   0,
	};

	const static size_t Nq = 3;
	static kvs::Real32 SphereCoordArray[Nq * 3] = {
		-170, -150, 150,  // center of strip lines
		 -60, -150, 150,  // center of uniline lines
		  50, -150, 150,  // center of poly lines
	};
	static kvs::UInt8 SphereColorArray[Nq * 3] = {
		255,   0,   0,
		  0, 255,   0,
		  0,   0, 255,
	};

	const static size_t Np = 9;
	static kvs::Real32 PointCoordArray[Np * 3] = {
		50, -50, 100,
		50,   0, 100,
		50,  50, 100,
		70, -50, 100,
		70,   0, 100,
		70,  50, 100,
		90, -50, 100,
		90,   0, 100,
		90,  50, 100,
	};

	static kvs::UInt8 PointColorArray[Np * 3] = {
		0, 0, 0,
		0, 0, 127,
		0, 0, 255,
		0, 127, 0,
		0, 127, 127,
		0, 127, 255,
		0, 255, 0,
		0, 255, 127,
		0, 255, 255,
	};

	static kvs::Real32 PointSizeArray[Np] = {
		2,4,8,
		4,8,16,
		8, 16, 32,
	};

	// 点オブジェクトを生成する関数
	kvs::PointObject* CreatePointObject()
	{
		// KVSの配列クラス（kvs::ValueArray）にセットする。
		kvs::ValueArray<kvs::Real32> coords(PointCoordArray, Np * 3);
		kvs::ValueArray<kvs::UInt8> colors(PointColorArray, Np * 3);
		kvs::ValueArray<kvs::Real32> sizes(PointSizeArray, Np);

		// 点オブジェクトを生成する。
		kvs::PointObject* object = new kvs::PointObject();
		object->setCoords(coords);
		object->setColors(colors);
		object->setSizes(sizes);

		// 生成した点オブジェクトのポインタを返す。
		return object;
	}

	kvs::PointObject* CreatePBRObject() {
		// KVSの配列クラス（kvs::ValueArray）にセットする。
		kvs::ValueArray<kvs::Real32> coords(PointCoordArray, Np * 3);
		kvs::ValueArray<kvs::UInt8> colors(PointColorArray, Np * 3);
		kvs::ValueArray<kvs::Real32> sizes(PointSizeArray, Np);

		for (int i = 0; i < coords.size(); i++)
		{
			int zi = i * 3 + 2;
			coords[zi] += 50.0;
		}

		// 点オブジェクトを生成する。
		kvs::PointObject* object = new kvs::PointObject();
		object->setCoords(coords);
		object->setColors(colors);
		object->setSizes(sizes);

		// 生成した点オブジェクトのポインタを返す。
		return object;
	}

	// 線分オブジェクト(Segment)を生成する関数
	kvs::LineObject* CreateLineObject()
	{
		// KVSの配列クラス（kvs::ValueArray）にセットする。
		kvs::ValueArray<kvs::Real32> coords(CoordArray, N * 3);
		kvs::ValueArray<kvs::UInt8> colors(ColorArray, N * 3);
		kvs::ValueArray<kvs::UInt32> connections(LineConnectionArray, L * 2);

		// 線分オブジェクトを生成する。
		kvs::LineObject* object = new kvs::LineObject();
		object->setLineType(kvs::LineObject::Segment);
		object->setColorType(kvs::LineObject::VertexColor);
		object->setCoords(coords);
		object->setColors(colors);
		object->setConnections(connections);
		object->setSize(2);

		// 生成した線分オブジェクトのポインタを返す。
		return object;
	}

	// 線分オブジェクト(Polyline)を生成する関数
	kvs::LineObject* CreatePolylineObject()
	{
		// KVSの配列クラス（kvs::ValueArray）にセットする。
		kvs::ValueArray<kvs::Real32> coords(PolylineCoordArray, Ny * 3);
		kvs::ValueArray<kvs::UInt8> colors(PolylineColorArray, Ny * 3);
		kvs::ValueArray<kvs::UInt32> connections(PolylineConnectionArray, Ly * 2);

		// 線分オブジェクトを生成する。
		kvs::LineObject* object = new kvs::LineObject();
		object->setLineType(kvs::LineObject::Polyline);
		object->setColorType(kvs::LineObject::VertexColor);
		object->setCoords(coords);
		object->setColors(colors);
		object->setConnections(connections);
		object->setSize(3);

		// 生成した線分オブジェクトのポインタを返す。
		return object;
	}

	// 線分オブジェクト(Uniline)を生成する関数
	kvs::LineObject* CreateUnilineObject()
	{
		// KVSの配列クラス（kvs::ValueArray）にセットする。
		kvs::ValueArray<kvs::Real32> coords(UnilineCoordArray, Nu * 3);
		kvs::ValueArray<kvs::UInt8> colors(UnilineColorArray, Lu * 3);
		kvs::ValueArray<kvs::UInt32> connections(UnilineConnectionArray, Lu + 1);

		// 線分オブジェクトを生成する。
		kvs::LineObject* object = new kvs::LineObject();
		object->setLineType(kvs::LineObject::Uniline);
		object->setColorType(kvs::LineObject::LineColor);
		object->setCoords(coords);
		object->setColors(colors);
		object->setConnections(connections);
		object->setSize(4);

		// 生成した線分オブジェクトのポインタを返す。
		return object;
	}

	// 線分オブジェクト(Strip)を生成する関数
	kvs::LineObject* CreateStripObject()
	{
		// KVSの配列クラス（kvs::ValueArray）にセットする。
		kvs::ValueArray<kvs::Real32> coords(StripCoordArray, Ns * 3);
		kvs::ValueArray<kvs::UInt8> colors(StripColorArray, (Ns - 1) * 3);
		//kvs::ValueArray<kvs::UInt32> connections(UnilineConnectionArray, Lu + 1);

		// 線分オブジェクトを生成する。
		kvs::LineObject* object = new kvs::LineObject();
		object->setLineType(kvs::LineObject::Strip);
		object->setColorType(kvs::LineObject::LineColor);
		object->setCoords(coords);
		object->setColors(colors);
		//object->setConnections(connections);
		object->setSize(5);

		// 生成した線分オブジェクトのポインタを返す。
		return object;
	}


	// ポリゴンオブジェクトを生成する関数
	kvs::PolygonObject* CreatePolygonObject()
	{
		// KVSの配列クラス（kvs::ValueArray）にセットする。
		kvs::ValueArray<kvs::Real32> coords(CoordArray, N * 3);
		kvs::ValueArray<kvs::UInt8> colors(ColorArray, N * 3);
		kvs::ValueArray<kvs::Real32> normals(NormalArray, N * 3);
		kvs::ValueArray<kvs::UInt32> connections(PolygonConnectionArray, P * 3);

		// ポリゴンオブジェクトを生成する。
		kvs::PolygonObject* object = new kvs::PolygonObject();
		object->setCoords(coords);
		object->setColors(colors);
		object->setNormals(normals);
		object->setConnections(connections);
		object->setPolygonType(kvs::PolygonObject::Triangle);
		object->setColorType(kvs::PolygonObject::VertexColor);
		object->setNormalType(kvs::PolygonObject::VertexNormal);

		// 生成したポリゴンオブジェクトのポインタを返す。
		return object;
	}

	kvs::UnstructuredVolumeObject* CreateGlyphObject()
	{
		kvs::ValueArray<kvs::Real32> coords(CoordArray, N * 3);
		kvs::ValueArray<kvs::UInt32> connections(GlyphConnectionArray, T * 4);
		kvs::ValueArray<kvs::Real32> values(GlyphValueArray, N * D);
		//std::string label = "TestGlyph";
		//std::string Unit = "[m]";

		kvs::UnstructuredVolumeObject* object = new kvs::UnstructuredVolumeObject();
		object->setCellType(kvs::UnstructuredVolumeObject::Tetrahedra);
		object->setVeclen(D);
		object->setNumberOfNodes(N);
		object->setNumberOfCells(T);
		object->setCoords(coords);
		object->setConnections(connections);
		object->setValues(values);

		return object;
	}

	kvs::UnstructuredVolumeObject* CreateDiamondGlyphObject()
	{
		kvs::ValueArray<kvs::Real32> coords(DiamondGlyphCoordArray, N * 3);
		kvs::ValueArray<kvs::UInt32> connections(GlyphConnectionArray, T * 4);
		kvs::ValueArray<kvs::Real32> values(GlyphValueArray, N * D);
		//std::string label = "TestGlyph";
		//std::string Unit = "[m]";

		kvs::UnstructuredVolumeObject* object = new kvs::UnstructuredVolumeObject();
		object->setCellType(kvs::UnstructuredVolumeObject::Tetrahedra);
		object->setVeclen(D);
		object->setNumberOfNodes(N);
		object->setNumberOfCells(T);
		object->setCoords(coords);
		object->setConnections(connections);
		object->setValues(values);

		return object;
	}

	kvs::PointObject* CreateSpherePointObject()
	{
		kvs::ValueArray<kvs::Real32> coords(SphereCoordArray, Nq * 3);
		kvs::ValueArray<kvs::UInt8> colors(SphereColorArray, Nq * 3);

		kvs::PointObject* object = new kvs::PointObject();
		object->setCoords(coords);
		object->setColors(colors);
		object->setSize(20);

		return object;
	}


	static std::vector<kvs::Real32> ParticleCoords;
	static std::vector<kvs::UInt8> ParticleColors;

	kvs::PointObject* CreateParticles()
	{

		float Size = 100.0;
		float width = 2.0;
		float density = 5.0;
		float distance = 1.0 / density;

		for (kvs::Real32 ix = 0.0; ix <= Size; ix += distance)
		{
			kvs::UInt8 red((ix / Size) * 255);

			for (kvs::Real32 iy = ix - width; iy <= ix + width; iy += distance)
			{
				kvs::UInt8 green((iy + width) / (width*2.0) * 255);
				//float green = (iy + width) / (width*2.0);

				if (iy<0.0 || iy > Size)
				{
					continue;
				}

				for (kvs::Real32 iz = ix - width; iz <= ix + width; iz += distance)
				{
					kvs::UInt8 blue((iz + width) / (width*2.0) * 255);
					//float blue = (iz + width) / (width*2.0);

					if (iz <0.0 || iz > Size)
					{
						continue;
					}

					ParticleCoords.push_back(ix + 100.0);
					ParticleCoords.push_back(iy);
					ParticleCoords.push_back(iz + 50.0);

					ParticleColors.push_back(red);
					ParticleColors.push_back(green);
					ParticleColors.push_back(blue);
				}
			}
		}

		kvs::PointObject* object = new kvs::PointObject();
		kvs::ValueArray<kvs::Real32> coords(ParticleCoords.data(), ParticleCoords.size());
		kvs::ValueArray<kvs::UInt8> colors(ParticleColors.data(), ParticleCoords.size());

		object->setCoords(coords);
		object->setColors(colors);
		object->setSize(20);

		return object;
	}
}

int main(int argc, char *argv[])
{
	kvs::oculus::jaea::Application app(argc, argv);
	kvs::oculus::jaea::Screen screen(&app);

#if 1
	// segment & vertex color
	kvs::LineObject* line_object = KVSMain::CreateLineObject();
	kvs::LineRenderer* line_renderer = new kvs::LineRenderer();
	screen.registerObject(line_object, line_renderer);
#endif

#if 1
	// polyline & vertex color
	kvs::LineObject* polyline_object = KVSMain::CreatePolylineObject();
	kvs::LineRenderer* polyline_renderer = new kvs::LineRenderer();
	screen.registerObject(polyline_object, polyline_renderer);
#endif

#if 1
	// uniline & line color
	kvs::LineObject* uniline_object = KVSMain::CreateUnilineObject();
	kvs::LineRenderer* uniline_renderer = new kvs::LineRenderer();
	screen.registerObject(uniline_object, uniline_renderer);
#endif

#if 1
	// strip & line color
	kvs::LineObject* strip_object = KVSMain::CreateStripObject();
	kvs::LineRenderer* strip_renderer = new kvs::LineRenderer();
	screen.registerObject(strip_object, strip_renderer);
#endif

#if 1
	kvs::PolygonObject* polygon_object = KVSMain::CreatePolygonObject();
	kvs::PolygonRenderer* polygon_renderer = new kvs::PolygonRenderer();
	screen.registerObject(polygon_object, polygon_renderer);
#endif

#if 1
	kvs::VolumeObjectBase* glyph_object = KVSMain::CreateGlyphObject();
	kvs::ArrowGlyph* glyph_renderer = new kvs::ArrowGlyph();
	glyph_renderer->setType(kvs::ArrowGlyph::TubeArrow);
	//glyph_renderer->setOpacityMode(kvs::ArrowGlyph::OpacityByMagnitude);
	screen.registerObject(glyph_object, glyph_renderer);
#endif

#if 1
	kvs::VolumeObjectBase* diamond_object = KVSMain::CreateDiamondGlyphObject();
	kvs::DiamondGlyph* dia_glyph_renderer = new kvs::DiamondGlyph();
	screen.registerObject(diamond_object, dia_glyph_renderer);
#endif

#if 1
	kvs::PointObject* sphere_object = KVSMain::CreateSpherePointObject();
	kvs::SphereGlyph* sphere_glyph_renderer = new kvs::SphereGlyph();
	screen.registerObject(sphere_object, sphere_glyph_renderer);
#endif

#if 1

#if 1
	kvs::PointObject * point_object = KVSMain::CreatePointObject();
#else
	kvs::Vector3ui resolution(8, 8, 8);
	kvs::StructuredVolumeObject* hydro_object = new kvs::HydrogenVolumeData(resolution);
	const size_t repetitions = 1;
	const float step = 0.5f;
	const kvs::TransferFunction tfunc(256);
	kvs::PointObject * point_object = new kvs::CellByCellMetropolisSampling(hydro_object, repetitions, step, tfunc);
	point_object->setSize(5);
#endif
	kvs::PointRenderer* point_renderer = new kvs::PointRenderer();
	screen.registerObject(point_object, point_renderer);
#endif

    kvs::oculus::jaea::TouchController event( &screen );

    event.setRotationFactor( 5000.0f );
    event.setTranslationFactor( 5000.0f );
    event.setScalingFactor( 30000.0f );

	screen.show();
	return app.run();
}