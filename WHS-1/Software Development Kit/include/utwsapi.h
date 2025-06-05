#pragma once

#include <windows.h>

#ifdef UTWSAPI_EXPORTS
#define UTWS_API __declspec(dllexport)
#else
#define UTWS_API __declspec(dllimport)
#endif

//�f�o�C�X���ʎq
//��M�@ RRD-1
#define UTWS_RRD_1    0x00000001
//���M�@ WHS-1
#define UTWS_WHS_1    0x00000002
// WHS-1�̃o�C�i���f�[�^
#define UTWS_WHS_1_BINARYDATA 0x00000003

//�G���[���ʎq�̃C���N���[�h
#include "utwserrdef.h"

//�\���̂̃C���N���[�h
#include "utwsstruct.h"

//////����////////////////////////////////

//�G���[����Ԃ�
//�߂�l
//���O�ɔ��������G���[�̃G���[���ʎq��Ԃ��܂��D
//���
//�����Ȉ����Ȃ�΁C�G���[���ʎq������Ԃ��܂��D
//�܂��C�����̃o�b�t�@�T�C�Y���G���[�����������������Ƃ��́C�o�b�t�@�T�C�Y�ɐ؂�l�߂��܂��D
UTWS_API UINT WINAPI UTWSGetErrorMessage(
  //�֐����s���̃n���h���Aopen�n�Aclose�n�̓n���h�����m�肵�Ȃ��̂�0�Ƃ���B
  /*in*/ HANDLE _handle,
  //�G���[�Ɋւ���������i�[����镶����o�b�t�@�ւ̃|�C���^
  /*out*/ LPSTR _err_message,
  //�w�肳�ꂽ�o�b�t�@�ɕێ��ł���ő啶�����D�I�[�� NULL �������܂�
  /*in*/ UINT _max_length
);

//�f�o�C�X���J��
//�߂�l
//��������ƃf�o�C�X�ւ̃n���h�����Ԃ�܂��D
//���s����� 0 ���Ԃ�܂��D
//32-bit DLL �ł�32bit�l�C64-bit DLL �ł�64bit�l�ł��邱�Ƃɒ��ӂ��Ă��������D
UTWS_API HANDLE WINAPI UTWSOpenDevice(
  //�I�[�v���������f�o�C�X�̎��ʎq���w�肵�Ă��������D
  /*in*/ UINT _device_id,
  //WHS1-1���I�[�v������Ƃ��́C���Ԗڂ̃f�o�C�X���I�[�v�����邩�w�肵�ĉ������D
  //�P�䂵���Ȃ����Ă��Ȃ��Ƃ��� 0�Ƃ��ĉ������D
  //RRD-1�ł�0�Ƃ��ĉ������D
  /*in*/ UINT _device_no
);

//�f�o�C�X���N���[�Y����
//�߂�l
//��������� TRUE ���Ԃ�D���s����� FALSE ���Ԃ�D
UTWS_API BOOL WINAPI UTWSCloseDevice(
  //�N���[�Y���悤�Ƃ���n���h��
  /*in*/ HANDLE _handle
);

//���ׂẴf�o�C�X���N���[�Y����
//�߂�l
//�Ȃ�
UTWS_API VOID WINAPI UTWSCloseAll(VOID);

//// RRD-1 //////////////////////////////////

//RRD-1�Ńf�[�^��M���J�n����
//�����Ȃ�� TRUE ���Ԃ�
//��2�����ɂ̓C�x���g�I�u�W�F�N�g�ւ̃|�C���^���w�肵�܂��D
//0 ���w�肳�ꂽ�ꍇ�C���̃I�u�W�F�N�g�͎g�p����܂���
//��M����ƁC�C�x���g�̓V�O�i����ԂɂȂ�܂��D
//�V�O�i����Ԃ́CUTWSRRD1GetData�֐��ł��ׂẴf�[�^�����o����Ȃ�����
//��V�O�i���ɂ͂Ȃ�܂���D
//��3�����ɂ̓R�[���o�b�N�֐��ւ̃|�C���^���w�肵�܂��D
//�R�[���o�b�N�֐��̑������̓��[�U��_callBackArg�Ŏw�肵�܂��D
//��M����ƁC���̃R�[���o�b�N�֐����Ăяo����܂��D
//��2�����́C�n���h�����Ԃ�܂��D
//�R�[���o�b�N�֐���typedef���ȉ��̒ʂ�Ƃ��܂�
typedef VOID(CALLBACK* UTWSRRD1CallBakFunc)(VOID*, VOID*);
UTWS_API BOOL WINAPI UTWSRRD1StartReceiving(
  //�f�o�C�X�̃n���h��
  /*in*/ HANDLE _handle,
  //�C�x���g�I�u�W�F�N�g�̃n���h���D�g�p���Ȃ��Ƃ��� 0
  /*in*/ HANDLE _event,
  //�R�[���o�b�N�֐��ւ̃|�C���^�D�g�p���Ȃ��Ƃ��� 0
  /*in*/ UTWSRRD1CallBakFunc _callBack,
  //�R�[���o�b�N�֐����Ăяo���Ƃ��̑�1�����D
  /*in*/ VOID* _callBackArg
);

//RRD-1�Ńf�[�^�̎�M�𒆎~����
//�����Ȃ�� TRUE ���Ԃ�.
UTWS_API BOOL CALLBACK UTWSRRD1StopReceiving(
  //�f�o�C�X�̃n���h��
  /*in*/ HANDLE _handle
);

//RRD-1�Ŏ�M�����f�[�^�����c���Ă��邩�Ԃ�
//�߂�l�@���s�Ȃ�� -1 ���Ԃ�D
UTWS_API UINT WINAPI UTWSRRD1DataCount(
  //�f�o�C�X�̃n���h��
  /*in*/ HANDLE _handle
);

//RRD-1�Ŏ�M�����f�[�^������o���D
//DLL���ł́C�ő�1024�̃f�[�^���o�b�t�@�ł���D
//���̊֐������s����ƁC���o���ꂽ�f�[�^���o�b�t�@�����苎����D
//���������ꍇ�� TRUE ��Ԃ��D
//�f�[�^�o�b�t�@�ɉ����Ȃ���ԂŌĂяo�����ƁCFALSE ���������C
//���C�u�����̃G���[�X�e�[�^�X�� UTWS_ERR_EMPTY_DATA �ɐݒ肳��܂��D
//���̂Ƃ���_data�̎w���o�b�t�@�ɕԂ����l�͕s��ł��D
//_data �� 0�̎��́C�������܂���D
UTWS_API BOOL WINAPI UTWSRRD1GetData(
  //�f�o�C�X�̃n���h��
  /*in*/ HANDLE _handle,
  //�f�[�^���󂯂�ƃo�b�t�@�i�\���́j�ւ̃|�C���^
  /*out*/ RRD1Data* _data
);

//RRD-1�Ŏ�M�����f�[�^������o���D
//UTWSRRD1GetData �̃~���b�P�ʃo�[�W����
UTWS_API BOOL WINAPI UTWSRRD1GetDataEx(
  //�f�o�C�X�̃n���h��
  /*in*/ HANDLE _handle,
  //�f�[�^���󂯂�ƃo�b�t�@�i�\���́j�ւ̃|�C���^
  /*out*/ RRD1DataEx* _data
);

//RRD-1���I�[�v�����Ă��邩�ǂ������ׂ�
//�߂�l�@�I�[�v�����Ă����TURE�C���Ă��Ȃ���� FALSE ��Ԃ��D
UTWS_API BOOL WINAPI UTWSRRD1IsOpen(
  //�f�o�C�X�̃n���h��
  /*in*/ HANDLE _handle
);

//RRD-1���I�[�v������USB�R�l�N�^������O���ꂽ�Ƃ��ɌĂяo�����
//�R�[���o�b�N�֐���o�^����D
//�����̏ꍇ�� TURE ���������D
UTWS_API BOOL WINAPI UTWSRRD1SetCallBackDisconnect(
  //�f�o�C�X�̃n���h��
  /*in*/ HANDLE _handle,
  //�R�[���o�b�N�֐��ւ̃|�C���^�D�g�p���Ȃ��Ƃ��� 0
  /*in*/ UTWSRRD1CallBakFunc _callBack,
  //�R�[���o�b�N�֐����Ăяo���Ƃ��̈���
  /*in*/ VOID* _callBackArg
);

//RRD-1�̖����A�h���X���擾����D
//��������� TRUE ��Ԃ��܂��D
//�����ɕ���������ĕԂ����̂�,�o�b�t�@�ɂ�11�����ȏ�m�ۂ��ĉ������D
UTWS_API BOOL WINAPI UTWSRRD1GetLocalAddress(
  //�f�o�C�X�̃n���h��
  /*in*/ HANDLE _handle,
  //���M��A�h���X���Ԃ����o�b�t�@�ւ̃|�C���^
  /*out*/ CHAR* _address
);

//RRD-1�̖����A�h���X��ݒ肵�܂��D
//���������TRUE��Ԃ��܂��D
//������0�^�[�~�l�[�^�[�̕�����ł��D
UTWS_API BOOL WINAPI UTWSRRD1SetLocalAddress(
  //�f�o�C�X�̃n���h��
  /*in*/ HANDLE _handle,
  //���M��A�h���X�ւ̃|�C���^
  /*in*/ const CHAR* _address
);

//// WHS-1 //////////////////////////////////
//�ڑ�����Ă���WHS-1�̑䐔��Ԃ��܂��D
//�߂�l : �ڑ����Ă���䐔�Ԃ��܂��D���s������ -1 ��Ԃ��܂��D
//_numConnected �ɐڑ��\��䐔���w�肵�ĉ������D
//_timeout �̊Ԃ� 20msec�Ԋu�Őڑ��䐔�𒲂ׂ܂��D
//�ڑ��\��䐔�ɒB�����炷���ɐ��䂪�߂�܂��D
//_numConnected��0�̂Ƃ��́C_timeout�̊ԃf�o�C�X�T���܂��D
//���̂Ƃ���_timeout��ɐ��䂪�߂�܂��D
UTWS_API UINT WINAPI UTWSWHS1CountConnected(
  //�ڑ��\��䐔�D����̂Ƃ��́@0
  /*in*/ UINT _numConnected,
  //�^�C���A�E�g�C�P��msec
  /*in*/ UINT _timeout
);

//WHS-1 �̐ݒ�l��ǂݎ��
//��������� TRUE ��Ԃ��D
UTWS_API BOOL WINAPI UTWSWHS1ReadConfig(
  //�f�o�C�X�̃n���h��
  /*in*/ HANDLE _handle,
  //�ݒ�����i�[����\���̂ւ̃|�C���^
  /*out*/ WHS1Config* _readConfig
);

//WHS-1 �̐ݒ�l����������
//��������� TRUE ��Ԃ��D
UTWS_API BOOL WINAPI UTWSWHS1WriteConfig(
  //�f�o�C�X�̃n���h��
  /*in*/ HANDLE _handle,
  //�ݒ�����i�[����\���̂ւ̃|�C���^
  /*in*/ const WHS1Config* _readConfig,
  //���������[�h�Ń������t���̂Ƃ� 0: �擪����㏑���C1: �t���ɂȂ����i�K�Ŏ~�߂�
  //���������[�h�ɐݒ肵�Ȃ��Ƃ��͖��������
  /*in*/ BYTE _mem_mode,
  //�ȉ����ݎ�����ݒ�.���������[�h�ɐݒ肵�Ȃ��Ƃ��͖��������̂� 0 �ł��܂�Ȃ��D
  /*in*/ WORD _year,
  /*in*/ WORD _month,
  /*in*/ WORD _day,
  /*in*/ WORD _hour,
  /*in*/ WORD _min,
  /*in*/ WORD _sec
);

//WHS-1�̖������M��A�h���X���擾���܂��D
//��������� TRUE ��Ԃ��܂��D
//�����ɕ���������ĕԂ����̂�,�o�b�t�@�ɂ�11�����ȏ�m�ۂ��ĉ������D
UTWS_API BOOL WINAPI UTWSWHS1GetDestinationAddress(
  //�f�o�C�X�̃n���h��
  /*in*/ HANDLE _handle,
  //���M��A�h���X���Ԃ����o�b�t�@�ւ̃|�C���^
  /*out*/ CHAR* _address
);

//WHS-1�̖������M��A�h���X��ݒ肵�܂��D
//���������TRUE��Ԃ��܂��D
//������0�^�[�~�l�[�^�[�̕�����ł��D
UTWS_API BOOL WINAPI UTWSWHS1SetDestinationAddress(
  //�f�o�C�X�̃n���h��
  /*in*/ HANDLE _handle,
  //���M��A�h���X�ւ̃|�C���^
  /*in*/ const CHAR* _address
);

//WHS-1 ���烁�����̓��e���_�E�����[�h�����C�u�������̃������ɕۑ����܂��D
//�߂�l: �ǂݍ��񂾃f�[�^����Ԃ��܂��D�G���[�̂Ƃ��̖߂�l��-1�ł��D
UTWS_API UINT WINAPI UTWSWHS1ReadMemory(
  //�f�o�C�X�̃n���h��
  /*in*/ HANDLE _handle
);

//WHS-1 ���烁�����̓��e���_�E�����[�h�����C�u�������̃������ɕۑ����܂��D
//�߂�l: �G���[�̂Ƃ��̖߂�l��FALSE�ł��D
UTWS_API BOOL WINAPI UTWSWHS1ReadMemoryEx(
  //�f�o�C�X�̃n���h��
  /*in*/ HANDLE _handle,
  //�ǂݏo�����f�[�^�̃w�b�_��
  /*out*/ WHS1MemDataHeader* _header
);

//WHS-1 ���烁�����̓��e���_�E�����[�h�����C�u�������̃������ɕۑ������f�[�^��
//�����o���D
//�߂�l : ���s�̂Ƃ�false
UTWS_API BOOL WINAPI UTWSWHS1GetReceivedMemory(
  //�f�o�C�X�̃n���h��
  /*in*/ HANDLE _handle,
  //�ǂݏo�����f�[�^�̃w�b�_��
  /*out*/ WHS1MemDataHeader* _header,
  //�ǂݏo�����f�[�^�̃f�[�^���CUTWSWHS1ReadMemory�֐��œ���ꂽ
  //�f�[�^�������̔z������炩���ߗp�ӂ��ĉ������D
  //0�Ȃ�΃f�[�^��ǂݍ��݂܂���D
  /*out*/ WHS1EcgData* _data,
  //�p�ӂ����z��WHS1EcgData[]�̃T�C�Y
  /*in*/ UINT _size,
  //TRUE�̂Ƃ�1���R�[�h������̎��v�𒲐����܂�
  /*in*/ BOOL _time_adjust
);
UTWS_API BOOL WINAPI UTWSWHS1GetReadedMemory(
  HANDLE _handle, WHS1MemDataHeader* _header, WHS1EcgData* _data, UINT _size, BOOL _time_adjust
);

//WHS-1 ���烁�����̓��e���_�E�����[�h�����C�u�������̃������ɕۑ������f�[�^��
//�t�@�C���ɏ����o��.
//�߂�l : ���s�̂Ƃ�FALSE
UTWS_API BOOL WINAPI UTWSWHS1SaveReceivedMemory(
  //�f�o�C�X�̃n���h��
  /*in*/ HANDLE _handle,
  //�p�X��
  /*in*/ LPCSTR _path,
  //TRUE�̂Ƃ�1���R�[�h������̎��v�𒲐����܂�
  /*in*/ BOOL _time_adjust
);
UTWS_API BOOL WINAPI UTWSWHS1SaveReadedMemory(
  HANDLE _handle, LPCSTR _path, BOOL _time_adjust
);

//WHS-1 ���烁�����̓��e���_�E�����[�h�����C�u�������̃������ɕۑ������f�[�^��
//�����o���D(�����x1�b�X�V���[�h���ɁC�{�����x�Ɓ[�����x��Ɨ����Ď擾�ł���j
//�߂�l : ���s�̂Ƃ�false
UTWS_API BOOL WINAPI UTWSWHS1GetReceivedMemoryEx(
  //�f�o�C�X�̃n���h��
  /*in*/ HANDLE _handle,
  //�ǂݏo�����f�[�^�̃w�b�_��
  /*out*/ WHS1MemDataHeader* _header,
  //�ǂݏo�����f�[�^�̃f�[�^���CUTWSWHS1ReadMemory�֐��œ���ꂽ
  //�f�[�^�������̔z������炩���ߗp�ӂ��ĉ������D
  //0�Ȃ�΃f�[�^��ǂݍ��݂܂���D
  /*out*/ WHS1EcgDataEx* _data,
  //�p�ӂ����z��WHS1EcgData[]�̃T�C�Y
  /*in*/ UINT _size,
  //TRUE�̂Ƃ�1���R�[�h������̎��v�𒲐����܂�
  /*in*/ BOOL _time_adjust
);

//WHS-1�̃o�[�W������Ԃ��D
UTWS_API BOOL WINAPI UTWSWHS1Version(
  //�f�o�C�X�̃n���h��
  /*in*/ HANDLE _handle,
  //�o�[�W����
  /*out*/ UINT* _ver
);

//WHS-1�ɋL�^���ꂽ�f�[�^�̃u���b�N����Ԃ�
//�߂�l : ���s�̂Ƃ�false
UTWS_API BOOL WINAPI UTWSWHS1GetRecordedSize(
  //�f�o�C�X�̃n���h��
  /*in*/ HANDLE _handle,
  //�f�[�^���L�^����Ă���u���b�N��
  /*out*/ UINT* _size_to_read
);

//WHS-1�̃t���b�V���������̓��e��񓯊��ŃC�i���Ŏ�荞��,
//�񓯊��Ȃ̂ŁA��荞�݊������C�x���g�I�u�W�F�N�g�܂��̓R�[���o�b�N�֐��Œ��ׂ�
//�߂�l:���ݍ��݊J�n�����s������FALSE
UTWS_API BOOL WINAPI UTWSWHS1ReadMemoryBinary(
  //�f�o�C�X�̃n���h��
  /*in*/ HANDLE _handle,
  //��荞�݊������ɃV�O�i����ԂƂȂ�C�x���g�̃n���h���B�g�p���Ȃ��Ȃ��0�Ƃ���B
  /*in*/ HANDLE _user_event,
  //��荞�݊������ɌĂяo���R�[���o�b�N�֐��B�g�p���Ȃ��Ȃ��0�Ƃ���B
  /*in*/ void (*_user_call_back)(void*),
  //�R�[���o�b�N�֐��̈����B
  void* _user_call_back_arg
);

//�o�C�i���Ŏ�荞�񂾃������̃o�C�g��
//getMemoryBinary�֐����s�O�ɁA�����œ���ꂽ�o�C�g���̃��������m�ۂ���
//�߂�l : ���s������FALSE�A���s����*_size�͕s��
UTWS_API BOOL WINAPI UTWSWHS1GetMemoryBinarySize(
  //�f�o�C�X�̃n���h��
  /*in*/ HANDLE _handle,
  //�o�C�g��
  /*out*/ UINT* _size
);

//��荞�񂾃o�C�i�����R�s�[����
//�擪��1byte�́AWHS-1��tempID���L�^����Ă���B
//�]���āA���ۂ̃���������1�o�C�g�����B
//�o�C�i���́A���߃G�X�P�[�v��u���ς݁B
//�܂��A256�o�C�g�u���b�N�̊J�n����0x01�A�I�[����0x04����сA�������I�[0xffffffff�͍폜�ς݁B
//�߂�l : _bufsize���ǂݍ��񂾃T�C�Y������������Ύ��s�AFALSE��Ԃ��B
UTWS_API BOOL WINAPI UTWSWHS1GetMemoryBinary(
  //�f�o�C�X�̃n���h��
  /*in*/ HANDLE _handle,
  //�R�s�[��̃o�b�t�@�A�h���X
  /*out*/ BYTE* _buf,
  //�o�b�t�@�̃T�C�Y
  /*in*/ UINT _bufsize
);

//�o�C�i�����t�@�C���֕ۑ�����
//�߂�l:���s��FALSE��Ԃ�
UTWS_API BOOL WINAPI UTWSWHS1SaveMemoryBinary(
  //�f�o�C�X�̃n���h��
  /*in*/ HANDLE _handle,
  //�ۑ���t�@�C����
  /*in*/ LPCSTR _filename
);

//�o�C�i�����[�h�̌��ʂ�Ԃ�
//�߂�l : �G���[���ʎq
UTWS_API UINT WINAPI UTWSWHS1ReadMemoryBinaryResult(
  //�f�o�C�X�̃n���h��
  /*in*/ HANDLE _handle
);

//�ǂݍ��ݒ��̃o�C�g���Ɠǂݍ��܂��\��̃o�C�g����Ԃ�
//�P�ʂ�1�u���b�N/256�o�C�g
//�߂�l:�@�ǂݍ��ݒ��ȊO��FALSE��Ԃ��B
//         ���̂Ƃ��� _seze_read��_size_for_read�̒l�𑀍삵�Ȃ�
UTWS_API BOOL WINAPI UTWSWHS1ReadSize(
  //�f�o�C�X�̃n���h��
  /*in*/ HANDLE _handle,
  //�ǂݍ��ݒ��̃u���b�N��
  /*out*/ UINT* _size_read,
  //�ǂݍ��܂��\��u���b�N��
  /*out*/ UINT* _size_for_read
);

//WHS-1���ؒf(USB�𔲂��Ȃ�)���ꂽ�Ƃ��ɌĂяo�����
//�R�[���o�b�N�֐���o�^����
//�����̏ꍇ�� TURE ���������D
UTWS_API BOOL WINAPI UTWSWHS1SetCallBackDisconnect(
  //�f�o�C�X�̃n���h��
  /*in*/ HANDLE _handle,
  //�R�[���o�b�N�֐��ւ̃|�C���^�D�g�p���Ȃ��Ƃ��� 0
  /*in*/ void (*_callBack)(void*),
  //�R�[���o�b�N�֐����Ăяo���Ƃ��̈���
  /*in*/ void* _callBackArg
);

//WHS-1�̘A������������擾����
//_days == 0�@��OFF
//�����̏ꍇ�� TRUE��Ԃ��D
UTWS_API BOOL WINAPI UTWSWHS1GetContinuousDays(
  //�f�o�C�X�̃n���h��
  /*in*/ HANDLE _handle,
  //�A������
  /*out*/ BYTE* _days
);

//WHS-1�̘A������ɐ���ݒ肷��
//_days == 0��OFF�C_days == [1-5]����ȊO��FALSE��Ԃ��D
//�����̏ꍇ��TRUE��Ԃ��D
UTWS_API BOOL WINAPI UTWSWHS1SetContinuousDays(
  //�f�o�C�X�̃n���h��
  /*in*/ HANDLE _handle,
  //�A������
  /*out*/ BYTE _days
);

///////////////////////////////////////////////////////////////////
//WHS-1�t���b�V���̃o�C�i������f�[�^�𕜌�����B
//�߂�l: �f�[�^����Ԃ��B���s�̂Ƃ�-1�B
//�o�C�i������f�[�^�𐶐�����B�f�[�^����Ԃ��B
UTWS_API UINT WINAPI UTWSWHS1BinaryToData(
  //�o�C�i���I�u�W�F�N�g�̃n���h��
  /*in*/  HANDLE _handle,
  //�o�C�i���f�[�^�ւ̃|�C���^
  /*in*/  BYTE* _binary,
  //�o�C�i���f�[�^�̃o�C�g��
  /*in*/  UINT  _size,
  //���莞����⊮�������Ƃ�TURE
  /*in*/  BOOL _time_adjust,
  //���U�[�u �펞FALSE
  /*in*/  BOOL _dtr_rri
);

//WHS-1�t���b�V���̃o�C�i����ۑ������t�@�C������A�f�[�^�𕜌�����B
//�߂�l: �f�[�^����Ԃ��B���s�̂Ƃ�-1�B
//�o�C�i������f�[�^�𐶐�����B�f�[�^����Ԃ��B
UTWS_API UINT WINAPI UTWSWHS1BinaryFileToData(
  //�o�C�i���I�u�W�F�N�g�̃n���h��
  /*in*/  HANDLE _handle,
  //�t�@�C����
  /*in*/  LPCTSTR _binaryfile_name,
  //���莞����⊮�������Ƃ�TURE
  /*in*/  BOOL _time_adjust,
  //���U�[�u �펞FALSE
  /*in*/  BOOL _dtr_rri
);

//�o�C�i���I�u�W�F�N�g�ŕ������ꂽ�f�[�^�����o���B
//�߂�l : ���s�̂Ƃ�FALSE
UTWS_API BOOL WINAPI UTWSWHS1GetDataFromBinary(
  //�o�C�i���I�u�W�F�N�g�̃n���h��
  /*in*/  HANDLE _handle,
  //�f�[�^�̃w�b�_���ւ̃o�b�t�@
  /*out*/ WHS1MemDataHeader* _header,
  //�f�[�^��ۑ�����z��ւ̃|�C���^
  //UTWSWHS1BinaryToData�֐��A�܂���UTWSWHS1BinaryFileToData�֐��œ���ꂽ
  //�f�[�^�������z�����������B
  /*out*/ WHS1EcgDataEx* _data,
  //_data��size
  /*in*/  UINT _size
);

//�o�C�i���I�u�W�F�N�g�ŕ������ꂽ�f�[�^���t�@�C����CSV�ۑ�����
//�߂�l : ���s�̂Ƃ�FALSE
UTWS_API BOOL WINAPI UTWSWHS1SaveDataFromBinary(
  //�o�C�i���f�[�^�I�u�W�F�N�g�̃n���h��
  /*in*/  HANDLE _handle,
  //�t�@�C����
  /*in*/  LPCSTR _file_name
);
