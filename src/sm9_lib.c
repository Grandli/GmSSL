/*
 *  Copyright 2014-2022 The GmSSL Project. All Rights Reserved.
 *
 *  Licensed under the Apache License, Version 2.0 (the License); you may
 *  not use this file except in compliance with the License.
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 */


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <gmssl/mem.h>
#include <gmssl/sm3.h>
#include <gmssl/sm9.h>
#include <gmssl/asn1.h>
#include <gmssl/error.h>


extern const sm9_bn_t SM9_ZERO;
extern const sm9_bn_t SM9_N;
extern const SM9_POINT *SM9_P1;
extern const SM9_TWIST_POINT *SM9_P2;

//把sm9的签名数据进行der编码
int sm9_signature_to_der(const SM9_SIGNATURE *sig, uint8_t **out, size_t *outlen)
{
	uint8_t hbuf[32];
	uint8_t Sbuf[65];
	size_t len = 0;

	sm9_fn_to_bytes(sig->h, hbuf);
	sm9_point_to_uncompressed_octets(&sig->S, Sbuf);

	if (asn1_octet_string_to_der(hbuf, sizeof(hbuf), NULL, &len) != 1
		|| asn1_bit_octets_to_der(Sbuf, sizeof(Sbuf), NULL, &len) != 1
		|| asn1_sequence_header_to_der(len, out, outlen) != 1
		|| asn1_octet_string_to_der(hbuf, sizeof(hbuf), out, outlen) != 1
		|| asn1_bit_octets_to_der(Sbuf, sizeof(Sbuf), out, outlen) != 1) {
		error_print();
		return -1;
	}
	return 1;
}

//从der编码数据获取SM9签名数据
int sm9_signature_from_der(SM9_SIGNATURE *sig, const uint8_t **in, size_t *inlen)
{
	int ret;
	const uint8_t *d;
	size_t dlen;
	const uint8_t *h;
	size_t hlen;
	const uint8_t *S;
	size_t Slen;

	if ((ret = asn1_sequence_from_der(&d, &dlen, in, inlen)) != 1) {
		if (ret < 0) error_print();
		return ret;
	}
	if (asn1_octet_string_from_der(&h, &hlen, &d, &dlen) != 1
		|| asn1_bit_octets_from_der(&S, &Slen, &d, &dlen) != 1
		|| asn1_check(hlen == 32) != 1
		|| asn1_check(Slen == 65) != 1
		|| asn1_length_is_zero(dlen) != 1) {
		error_print();
		return -1;
	}
	if (sm9_fn_from_bytes(sig->h, h) != 1
		|| sm9_point_from_uncompressed_octets(&sig->S, S) != 1) {
		error_print();
		return -1;
	}
	return 1;
}

char sign_preHex[5][65 * 12];
//为了兼容不同的SM9主密钥系统，使用preComputeFlg来处理，支持5个不同系统的运算
int sm9_sign_reCompute(SM9_SIGN_PUBLIC_KEY signPub, int preComputeFlg)
{
    sm9_fp12_t g;
    //只支持5个（0表示不进行预运算）
    if(preComputeFlg<=0||preComputeFlg>5)
    {
        error_print();
        return -1;
    }
    // g = e(P1, Ppubs) (for sign)
    sm9_pairing(g, &signPub, SM9_P1);
    sm9_fp12_to_hex(g, sign_preHex[preComputeFlg-1]);
    return preComputeFlg;
}

//初始化签名上下文
int sm9_sign_init(SM9_SIGN_CTX *ctx, int preCompute)
{
	const uint8_t prefix[1] = { SM9_HASH2_PREFIX };
	sm3_init(&ctx->sm3_ctx);
    ctx->preCompute = preCompute;
	sm3_update(&ctx->sm3_ctx, prefix, sizeof(prefix));
	return 1;
}

//签名处理hash数据
int sm9_sign_update(SM9_SIGN_CTX *ctx, const uint8_t *data, size_t datalen)
{
	sm3_update(&ctx->sm3_ctx, data, datalen);
	return 1;
}

//完成数据签名
int sm9_sign_finish(SM9_SIGN_CTX *ctx, const SM9_SIGN_KEY *key, uint8_t *sig, size_t *siglen)
{
	SM9_SIGNATURE signature;

	if (sm9_do_sign(key, &ctx->sm3_ctx, &signature, ctx->preCompute) != 1) {
		error_print();
		return -1;
	}
	*siglen = 0;
    //把签名数据转为der编码
	if (sm9_signature_to_der(&signature, &sig, siglen) != 1) {
		error_print();
		return -1;
	}
	return 1;
}

//SM9签名运算
int sm9_do_sign(const SM9_SIGN_KEY *key, const SM3_CTX *sm3_ctx, SM9_SIGNATURE *sig, int preCompute)
{
	sm9_fn_t r;
	sm9_fp12_t g;
	uint8_t wbuf[32 * 12];
	SM3_CTX ctx = *sm3_ctx;
	SM3_CTX tmp_ctx;
	uint8_t ct1[4] = {0,0,0,1};
	uint8_t ct2[4] = {0,0,0,2};
	uint8_t Ha[64];

	// A1: g = e(P1, Ppubs)
    if(preCompute==0)
    {
        sm9_pairing(g, &key->Ppubs, SM9_P1);
    }
    else
    {
        sm9_fp12_from_hex(g, sign_preHex[preCompute-1]);
    }


	do {
		// A2: rand r in [1, N-1]
		if (sm9_fn_rand(r) != 1) {
			error_print();
			return -1;
		}
		//sm9_fn_from_hex(r, "00033C8616B06704813203DFD00965022ED15975C662337AED648835DC4B1CBE"); // for testing

		// A3: w = g^r
		sm9_fp12_pow(g, g, r);
		sm9_fp12_to_bytes(g, wbuf);

		// A4: h = H2(M || w, N)
		sm3_update(&ctx, wbuf, sizeof(wbuf));
		tmp_ctx = ctx;
		sm3_update(&ctx, ct1, sizeof(ct1));
		sm3_finish(&ctx, Ha);
		sm3_update(&tmp_ctx, ct2, sizeof(ct2));
		sm3_finish(&tmp_ctx, Ha + 32);
		sm9_fn_from_hash(sig->h, Ha);

		// A5: l = (r - h) mod N, if l = 0, goto A2
		sm9_fn_sub(r, r, sig->h);

	} while (sm9_fn_is_zero(r));

	// A6: S = l * dsA
	sm9_point_mul(&sig->S, r, &key->ds);

	gmssl_secure_clear(&r, sizeof(r));
	gmssl_secure_clear(&g, sizeof(g));
	gmssl_secure_clear(wbuf, sizeof(wbuf));
	gmssl_secure_clear(&tmp_ctx, sizeof(tmp_ctx));
	gmssl_secure_clear(Ha, sizeof(Ha));

	return 1;
}

//开始SM9验签
int sm9_verify_init(SM9_SIGN_CTX *ctx, int preCompute)
{
	const uint8_t prefix[1] = { SM9_HASH2_PREFIX };
	sm3_init(&ctx->sm3_ctx);
    ctx->preCompute = preCompute;
	sm3_update(&ctx->sm3_ctx, prefix, sizeof(prefix));
	return 1;
}

//SM9验签处理消息的数据
int sm9_verify_update(SM9_SIGN_CTX *ctx, const uint8_t *data, size_t datalen)
{
	sm3_update(&ctx->sm3_ctx, data, datalen);
	return 1;
}

//完成SM9验签运算
int sm9_verify_finish(SM9_SIGN_CTX *ctx, const uint8_t *sig, size_t siglen,
	const SM9_SIGN_PUBLIC_KEY *signPublicKey, const char *id, size_t idlen)
{
	int ret;
	SM9_SIGNATURE signature;

	if (sm9_signature_from_der(&signature, &sig, &siglen) != 1
		|| asn1_length_is_zero(siglen) != 1) {
		error_print();
		return -1;
	}

	if ((ret = sm9_do_verify(signPublicKey, id, idlen, &ctx->sm3_ctx, &signature, ctx->preCompute)) < 0) {
		error_print();
		return -1;
	}
	return ret;
}

//SM9验签运算
int sm9_do_verify(const SM9_SIGN_PUBLIC_KEY *signPublicKey, const char *id, size_t idlen,
	const SM3_CTX *sm3_ctx, const SM9_SIGNATURE *sig, int preCompute)
{
	sm9_fn_t h1;
	sm9_fn_t h2;
	sm9_fp12_t g;
	sm9_fp12_t t;
	sm9_fp12_t u;
	sm9_fp12_t w;
	SM9_TWIST_POINT P;
	uint8_t wbuf[32 * 12];
	SM3_CTX ctx = *sm3_ctx;
	SM3_CTX tmp_ctx;
	uint8_t ct1[4] = {0,0,0,1};
	uint8_t ct2[4] = {0,0,0,2};
	uint8_t Ha[64];

	// B1: check h in [1, N-1]
    if (sm9_bn_is_zero(sig->h) == 1
        || sm9_bn_cmp(sig->h, SM9_N) >= 0) {
        error_print();
        return -1;
    }

	// B2: check S in G1
    if(!sm9_point_is_on_curve(&sig->S))
    {
        error_print();
        return -1;
    }

	// B3: g = e(P1, Ppubs)
    if(preCompute==0)
    {
        sm9_pairing(g, signPublicKey, SM9_P1);
    }
    else
    {
        sm9_fp12_from_hex(g, sign_preHex[preCompute-1]);
    }

	// B4: t = g^h
	sm9_fp12_pow(t, g, sig->h);

	// B5: h1 = H1(ID || hid, N)
	sm9_hash1(h1, id, idlen, SM9_HID_SIGN);

	// B6: P = h1 * P2 + Ppubs
	sm9_twist_point_mul_generator(&P, h1);
	sm9_twist_point_add_full(&P, &P, signPublicKey);

	// B7: u = e(S, P)
	sm9_pairing(u, &P, &sig->S);

	// B8: w = u * t
	sm9_fp12_mul(w, u, t);
	sm9_fp12_to_bytes(w, wbuf);

	// B9: h2 = H2(M || w, N), check h2 == h
	sm3_update(&ctx, wbuf, sizeof(wbuf));
	tmp_ctx = ctx;
	sm3_update(&ctx, ct1, sizeof(ct1));
	sm3_finish(&ctx, Ha);
	sm3_update(&tmp_ctx, ct2, sizeof(ct2));
	sm3_finish(&tmp_ctx, Ha + 32);
	sm9_fn_from_hash(h2, Ha);
	if (sm9_fn_equ(h2, sig->h) != 1) {
		return 0;
	}

	return 1;
}

char enc_preHex[5][65 * 12];
//sm9加解密的预运算
//为了兼容不同的SM9主密钥系统，使用preComputeFlg来处理，支持5个不同系统的运算
int sm9_enc_reCompute(SM9_ENC_PUBLIC_KEY encPub, int preComputeFlg)
{
    sm9_fp12_t g;
    //只支持5个（0表示不进行预运算）
    if(preComputeFlg<=0||preComputeFlg>5)
    {
        error_print();
        return -1;
    }
    //  g = e(Ppube, P2) for enc
    sm9_pairing(g, SM9_P2, &encPub);
    sm9_fp12_to_hex(g, enc_preHex[preComputeFlg-1]);

    return preComputeFlg;
}

//SM9加密封装密钥计算
int sm9_kem_encrypt(const SM9_ENC_PUBLIC_KEY *encPublicKey, const char *id, size_t idlen,
	size_t klen, uint8_t *kbuf, SM9_POINT *C, int preCompute)
{
	sm9_fn_t r;
	sm9_fp12_t g;
	sm9_fp12_t w;
	uint8_t wbuf[32 * 12];
	uint8_t cbuf[65];
	SM3_KDF_CTX kdf_ctx;

	// A1: Q = H1(ID||hid,N) * P1 + Ppube
	sm9_hash1(r, id, idlen, SM9_HID_ENC);
	sm9_point_mul(C, r, SM9_P1);
	sm9_point_add(C, C, encPublicKey);

	do {
		// A2: rand r in [1, N-1]
		if (sm9_fn_rand(r) != 1) {
			error_print();
			return -1;
		}

		// A3: C1 = r * Q
		sm9_point_mul(C, r, C);
        //把C1转为字符串
		sm9_point_to_uncompressed_octets(C, cbuf);

		// A4: g = e(Ppube, P2)
        if(preCompute==0)
        {
            sm9_pairing(w, SM9_P2, encPublicKey);
        }
        else
        {
            sm9_fp12_from_hex(w, enc_preHex[preCompute-1]);
        }

        //把g赋值给w
        //sm9_fp12_copy(w, g);

		// A5: w = g^r
		sm9_fp12_pow(w, w, r);
		sm9_fp12_to_bytes(w, wbuf);

		// A6: K = KDF(C || w || ID_B, klen), if K == 0, goto A2
		sm3_kdf_init(&kdf_ctx, klen);
		sm3_kdf_update(&kdf_ctx, cbuf + 1, 64);
		sm3_kdf_update(&kdf_ctx, wbuf, sizeof(wbuf));
		sm3_kdf_update(&kdf_ctx, (uint8_t *)id, idlen);
		sm3_kdf_finish(&kdf_ctx, kbuf);

	} while (mem_is_zero(kbuf, klen) == 1);

    //清理相关内存数据
	gmssl_secure_clear(&r, sizeof(r));
	gmssl_secure_clear(&w, sizeof(w));
	gmssl_secure_clear(wbuf, sizeof(wbuf));
	gmssl_secure_clear(&kdf_ctx, sizeof(kdf_ctx));

	// A7: output (K, C)
	return 1;
}

//SM9解密封装密钥计算
int sm9_kem_decrypt(const SM9_ENC_KEY *key, const char *id, size_t idlen, const SM9_POINT *C,
	size_t klen, uint8_t *kbuf)
{
	sm9_fp12_t w;
	uint8_t wbuf[32 * 12];
	uint8_t cbuf[65];
	SM3_KDF_CTX kdf_ctx;

	// B1: check C in G1
	sm9_point_to_uncompressed_octets(C, cbuf);

	// B2: w = e(C, de);  此步是必须的
	sm9_pairing(w, &key->de, C);
	sm9_fp12_to_bytes(w, wbuf);

	// B3: K = KDF(C || w || ID, klen)
	sm3_kdf_init(&kdf_ctx, klen);
	sm3_kdf_update(&kdf_ctx, cbuf + 1, 64);
	sm3_kdf_update(&kdf_ctx, wbuf, sizeof(wbuf));
	sm3_kdf_update(&kdf_ctx, (uint8_t *)id, idlen);
	sm3_kdf_finish(&kdf_ctx, kbuf);

	if (mem_is_zero(kbuf, klen)) {
		error_print();
		return -1;
	}

    //清理相关内存数据
	gmssl_secure_clear(&w, sizeof(w));
	gmssl_secure_clear(wbuf, sizeof(wbuf));
	gmssl_secure_clear(&kdf_ctx, sizeof(kdf_ctx));

	// B4: output K
	return 1;
}

//sm9加密运算
int sm9_do_encrypt(const SM9_ENC_PUBLIC_KEY *encPublicKey, const char *id, size_t idlen,
	const uint8_t *in, size_t inlen,
	SM9_POINT *C1, uint8_t *c2, uint8_t c3[SM3_HMAC_SIZE], int preCompute)
{
	SM3_HMAC_CTX hmac_ctx;
	//uint8_t K[SM9_MAX_PLAINTEXT_SIZE + 32];
    uint8_t *K = (uint8_t *)malloc(inlen+SM3_HMAC_SIZE);

    //计算加密封装密钥
	if (sm9_kem_encrypt(encPublicKey, id, idlen, inlen+SM3_HMAC_SIZE, K, C1, preCompute) != 1) {
		error_print();
        free(K);
		return -1;
	}
	gmssl_memxor(c2, K, in, inlen);

	//sm3_hmac(K + inlen, 32, c2, inlen, c3);
	sm3_hmac_init(&hmac_ctx, K+inlen, SM3_HMAC_SIZE);
	sm3_hmac_update(&hmac_ctx, c2, inlen);
	sm3_hmac_finish(&hmac_ctx, c3);
	gmssl_secure_clear(&hmac_ctx, sizeof(hmac_ctx));
    free(K);
	return 1;
}

//sm9解密运算
int sm9_do_decrypt(const SM9_ENC_KEY *key, const char *id, size_t idlen,
	const SM9_POINT *C1, const uint8_t *c2, size_t c2len, const uint8_t c3[SM3_HMAC_SIZE],
	uint8_t *out)
{
	SM3_HMAC_CTX hmac_ctx;
	//uint8_t K[SM9_MAX_PLAINTEXT_SIZE + SM3_HMAC_SIZE];
	uint8_t mac[SM3_HMAC_SIZE];
    uint8_t *K = (uint8_t *)malloc(c2len+SM3_HMAC_SIZE);
    //计算解密封装密钥
	if (sm9_kem_decrypt(key, id, idlen, C1, c2len+SM3_HMAC_SIZE, K) != 1) {
		error_print();
        free(K);
		return -1;
	}
	//sm3_hmac(k + c2len, SM3_HMAC_SIZE, c2, c2len, mac);
	sm3_hmac_init(&hmac_ctx, K + c2len, SM3_HMAC_SIZE);
	sm3_hmac_update(&hmac_ctx, c2, c2len);
	sm3_hmac_finish(&hmac_ctx, mac);
	gmssl_secure_clear(&hmac_ctx, sizeof(hmac_ctx));

	if (gmssl_secure_memcmp(c3, mac, sizeof(mac)) != 0) {
		error_print();
        free(K);
		return -1;
	}
	gmssl_memxor(out, K, c2, c2len);
    //printf("sm9_do_decrypt out[%ld] = %s\n", c2len, out);
    free(K);
	return 1;
}

//SDK中SM9使用的是XOR的加密方式
#define SM9_ENC_TYPE_XOR	0
#define SM9_ENC_TYPE_ECB	1
#define SM9_ENC_TYPE_CBC	2
#define SM9_ENC_TYPE_OFB	4
#define SM9_ENC_TYPE_CFB	8

/*
SM9Cipher ::= SEQUENCE {
	EnType		INTEGER, -- 0 for XOR
	C1		BIT STRING, -- uncompressed octets of ECPoint
	C3		OCTET STRING, -- 32 bytes HMAC-SM3 tag
	CipherText	OCTET STRING,
}
*/
//对密文进行der编码
int sm9_ciphertext_to_der(const SM9_POINT *C1, const uint8_t *c2, size_t c2len,
	const uint8_t c3[SM3_HMAC_SIZE], uint8_t **out, size_t *outlen)
{
	int en_type = SM9_ENC_TYPE_XOR;
	uint8_t c1[65];
	size_t len = 0;

	if (sm9_point_to_uncompressed_octets(C1, c1) != 1) {
		error_print();
		return -1;
	}
	if (
        //预计算长度
        asn1_int_to_der(en_type, NULL, &len) != 1
		|| asn1_bit_octets_to_der(c1, sizeof(c1), NULL, &len) != 1
		|| asn1_octet_string_to_der(c3, SM3_HMAC_SIZE, NULL, &len) != 1
		|| asn1_octet_string_to_der(c2, c2len, NULL, &len) != 1
        //开始输出，设置总长度
		|| asn1_sequence_header_to_der(len, out, outlen) != 1
        //根据数据类型进行封装
		|| asn1_int_to_der(en_type, out, outlen) != 1
		|| asn1_bit_octets_to_der(c1, sizeof(c1), out, outlen) != 1
		|| asn1_octet_string_to_der(c3, SM3_HMAC_SIZE, out, outlen) != 1
		|| asn1_octet_string_to_der(c2, c2len, out, outlen) != 1) {
		error_print();
		return -1;
	}
	return 1;
}
//把der编码转为密文
int sm9_ciphertext_from_der(SM9_POINT *C1, const uint8_t **c2, size_t *c2len,
	const uint8_t **c3, const uint8_t **in, size_t *inlen)
{
	int ret;
	const uint8_t *d;
	size_t dlen;
	int en_type;
	const uint8_t *c1;
	size_t c1len;
	size_t c3len;

	if ((ret = asn1_sequence_from_der(&d, &dlen, in, inlen)) != 1) {
		if (ret < 0) error_print();
		return ret;
	}
	if (asn1_int_from_der(&en_type, &d, &dlen) != 1
		|| asn1_bit_octets_from_der(&c1, &c1len, &d, &dlen) != 1
		|| asn1_octet_string_from_der(c3, &c3len, &d, &dlen) != 1
		|| asn1_octet_string_from_der(c2, c2len, &d, &dlen) != 1
		|| asn1_length_is_zero(dlen) != 1) {
		error_print();
		return -1;
	}
	if (en_type != SM9_ENC_TYPE_XOR) {
		error_print();
		return -1;
	}
	if (c1len != 65) {
		error_print();
		return -1;
	}
	if (c3len != SM3_HMAC_SIZE) {
		error_print();
		return -1;
	}
    //内部有验证点是否在SM9的椭圆曲线上
	if (sm9_point_from_uncompressed_octets(C1, c1) != 1) {
		error_print();
		return -1;
	}
	return 1;
}

//SM9加密接口
int sm9_encrypt(const SM9_ENC_PUBLIC_KEY *encPublicKey, const char *id, size_t idlen,
	const uint8_t *in, size_t inlen, uint8_t *out, size_t *outlen, int preCompute)
{
	SM9_POINT C1;
	uint8_t *c2;//[SM9_MAX_PLAINTEXT_SIZE];
	uint8_t c3[SM3_HMAC_SIZE];

//	if (inlen > SM9_MAX_PLAINTEXT_SIZE) {
//		error_print();
//		return -1;
//	}
    c2 = (uint8_t *)malloc(inlen+SM3_HMAC_SIZE);
    //加密运算
	if (sm9_do_encrypt(encPublicKey, id, idlen, in, inlen, &C1, c2, c3, preCompute) != 1) {
        free(c2);
		error_print();
		return -1;
	}
	*outlen = 0;
    //对C1，C2，C3等信息进行der编码
	if (sm9_ciphertext_to_der(&C1, c2, inlen, c3, &out, outlen) != 1) { // FIXME: when out == NULL
        free(c2);
		error_print();
		return -1;
	}
    free(c2);
	return 1;
}

//SM9解密接口
int sm9_decrypt(const SM9_ENC_KEY *key, const char *id, size_t idlen,
	const uint8_t *in, size_t inlen, uint8_t *out, size_t *outlen)
{
	SM9_POINT C1;
	const uint8_t *c2;
	size_t c2len;
	const uint8_t *c3;
    //从der编码中获取C1，C2，C3等信息
	if (sm9_ciphertext_from_der(&C1, &c2, &c2len, &c3, &in, &inlen) != 1
		|| asn1_length_is_zero(inlen) != 1) {
		error_print();
		return -1;
	}
	*outlen = c2len;
    //printf("c2len = %d\n", c2len);
	if (!out) {
		return 1;
	}
    //解密运算
	if (sm9_do_decrypt(key, id, idlen, &C1, c2, c2len, c3, out) != 1) {
		error_print();
		return -1;
	}
	return 1;
}
