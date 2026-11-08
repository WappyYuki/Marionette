class InGamePlay;

// draw.cpp
void DrawScene( InGamePlay &game );

// projview.cpp
void Viewing( const InGamePlay &game );
void Projection( void );


#define NUM_BALLOON 100

//-------- common window parameters
typedef struct {
    int xo;
    int yo;
    int width;
    int height;
    float aspect;
	char *title;
} WindowDataT;

//// オブジェクト用の共通構造体
typedef struct {
	float x, y, z; //位置座標
	float roll, pitch, yaw; //姿勢（回転）情報
	bool visuable;
	int state; //状態変数
} ObjDataT;

//-------- simulation data
typedef struct {
    //////// ユーザ定義のシーンについてのデータをここにまとめる
	ObjDataT camera0; //カメラ0オブジェクト(主観視点）
	ObjDataT camera1; //カメラ1オブジェクト(客観視点）
	ObjDataT camera;
	// プレイヤーは Player クラス（player.h）に移した

	float move; //カメラの移動量
	float rot; //カメラの回転量

	bool mouse_left;    //マウスの左ボタンの状態
	bool mouse_right;  //マウスの右ボタンの状態
	bool mouse_middle;  //マウスの中ボタンの状態


	float mouse_x; //正規化されたマウスのX座標[-1.0, 1.0](右側が正)
	float mouse_y; //正規化されたマウスのY座標[-1.0, 1.0](左側が正)

	////////
} SimDataT;
