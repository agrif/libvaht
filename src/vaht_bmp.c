#include "vaht_intern.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* The Riven BMP format is ugly as hell -- forgive the macros */
#define COLOR(ti, col) (bmp->table[(ti * 3) + col])
#define LOOKBACK(num, spin) (decoded[si - (num * 2) + (spin)])
#define LOOKBACKP(m) (decoded[(long)si - (long)m])
#define R 0
#define G 1
#define B 2
#define EMIT(c) bmp->data[3 * i] = COLOR(c, R); bmp->data[3 * i + 1] = COLOR(c, G); bmp->data[3 * i + 2] = COLOR(c, B); bmp->idata[i] = c; ++i;
#define EMITI(ind) decoded[i] = (ind); ++i;
#define READC(v) vaht_resource_read(bmp->res, sizeof(uint8_t), &v);

vaht_bmp* vaht_bmp_open(vaht_resource* resource)
{
	if (strcmp(vaht_resource_type(resource), "tBMP") != 0)
		return NULL;
	
	vaht_bmp* ret = malloc(sizeof(vaht_bmp));
	ret->format = 0;
	struct vaht_bmp_header header;
	
	vaht_resource_seek(resource, 0);
	vaht_resource_read(resource, sizeof(struct vaht_bmp_header), &header);
	VAHT_SWAP_U16(header.width);
	VAHT_SWAP_U16(header.height);
	VAHT_SWAP_U16(header.bytes_per_row);
	
	ret->width = header.width;
	ret->height = header.height;
	ret->bytes_per_row = header.bytes_per_row;
	ret->data = NULL;
	ret->table = NULL;
	ret->idata = NULL;
	ret->datastart = vaht_resource_tell(resource);
	ret->res = resource;
	
	vaht_resource_grab(ret->res);
	
	if (header.truecolor == 4)
	{
		ret->format |= 0x2;
	}
	if (header.compression == 4)
	{
		ret->format |= 0x1;
	}
	
	return ret;
}

uint8_t* vaht_bmp_data(vaht_bmp* bmp)
{
	if (bmp->data)
		return bmp->data;
	
	vaht_resource_seek(bmp->res, bmp->datastart);
	bmp->data = malloc(sizeof(uint8_t) * bmp->width * bmp->height * 3);
	
	uint8_t t1, t2, t3;
	unsigned int si, ut1, ut2;
	
	if (vaht_bmp_truecolor(bmp))
	{
		for (ut2 = 0; ut2 < bmp->height; ++ut2)
		{
			for (ut1 = 0; ut1 < bmp->width; ++ut1)
			{
				READC(t3);
				READC(t2);
				READC(t1);
				bmp->data[(3 * ((ut2 * bmp->width) + ut1)) + 0] = t1;
				bmp->data[(3 * ((ut2 * bmp->width) + ut1)) + 1] = t2;
				bmp->data[(3 * ((ut2 * bmp->width) + ut1)) + 2] = t3;
			}
			vaht_resource_read(bmp->res, sizeof(uint8_t) * bmp->width, NULL);
		}
		return bmp->data;
	}
	
	uint32_t u0;
	vaht_resource_read(bmp->res, sizeof(uint32_t), &u0);
	VAHT_SWAP_U16(u0); /* should now be 0x030418FF */
	bmp->table = malloc(sizeof(uint8_t) * 256 * 3);
	vaht_resource_read(bmp->res, sizeof(uint8_t) * 256 * 3, bmp->table);
	/* table is in BGR, BGR, BGR format, like truecolor
	   so we need to convert it to RGB like sane people */
	unsigned int i;
	for (i = 0; i < 256; i++)
	{
		uint32_t r = bmp->table[3 * i];
		bmp->table[3 * i] = bmp->table[3 * i + 2];
		bmp->table[3 * i + 2] = r;
	}

	i = 0;
	uint8_t c;
	bmp->idata = malloc(sizeof(uint8_t) * bmp->width * bmp->height);
	
	if (!vaht_bmp_compressed(bmp))
	{
		while (i < bmp->width * bmp->height)
		{
			if (i % bmp->width == 0 && i != 0)
			{
				ut1 = bmp->bytes_per_row - bmp->width;
				while (ut1 > 0)
				{
					READC(c);
					--ut1;
				}
			}		
			READC(c);
			EMIT(c);
		}
		return bmp->data;
	}
	
	/* first 4 bytes are unknown, so ignore */
	READC(c);
	READC(c);
	READC(c);
	READC(c);
	
	unsigned int scc = 0; /* sub-command count, 0 when normal commands */
	int st1;
	uint8_t* decoded = malloc((bmp->height * bmp->bytes_per_row * sizeof(uint8_t)));
	while (i < bmp->height * bmp->bytes_per_row)
	{
		READC(c);
		si = i;
		if (c == 0x00 && scc == 0)
		{
			/* end of picture */
			break;
		} else if (c >= 0x01 && c <= 0x3f && scc == 0) {
			/* output (command) duplets, data follows */
			while (c != 0)
			{
				READC(t1);
				EMITI(t1);
				READC(t1);
				EMITI(t1);
				--c;
			}
		} else if (c >= 0x40 && c <= 0x7f && scc == 0) {
			/* repeat last duplet (c & 0x3f) times */
			c &= 0x3f;
			while (c != 0)
			{
				EMITI(LOOKBACK(1, 0));
				EMITI(LOOKBACK(1, 1));
				--c;
			}
		} else if (c >= 0x80 && c <= 0xbf && scc == 0) {
			/* repeat last 2 duplets (c & 0x3f) times */
			c &= 0x3f;
			while (c != 0)
			{
				EMITI(LOOKBACK(2, 0));
				EMITI(LOOKBACK(2, 1));
				EMITI(LOOKBACK(1, 0));
				EMITI(LOOKBACK(1, 1));
				--c;
			}
		} else if (c >= 0xc0 && scc == 0) {
			/* start a subcommand stream */
			scc = (c & 0x3f) + 1; /* counteracts last line */
			
			/* start subcommands */
		} else if ((c & 0xf0) == 0x00 && scc > 0) {
			/* 0000mmmm
			 * repeat duplet m away (1 == last)
			 */
			c = (c & 0x0f);
			EMITI(LOOKBACK(c, 0));
			EMITI(LOOKBACK(c, 1));
		} else if (c == 0x10 && scc > 0) {
			/* 0x10 p
			 * last duplet, second is p
			 */
			EMITI(LOOKBACK(1, 0));
			READC(c);
			EMITI(c);
		} else if ((c & 0xf0) == 0x10 && c != 0x10 && scc > 0) {
			/* 0001mmmm
			 * repeat last duplet, second is pixel -m
			 */
			c = (c & 0x0f);
			EMITI(LOOKBACK(1, 0));
			EMITI(LOOKBACKP(c + 1)); /* deal with non-increment lookbacks */
		} else if ((c & 0xf0) == 0x20 && scc > 0) {
			/* 0010xxxx
			 * last duplet, +x on 2
			 */
			c = (c & 0x0f);
			EMITI(LOOKBACK(1, 0));
			EMITI(LOOKBACK(1, 1) + c);
		} else if ((c & 0xf0) == 0x30 && scc > 0) {
			/* 0011xxxx
			 * last duplet, -x on 2
			 */
			c = (c & 0x0f);
			EMITI(LOOKBACK(1, 0));
			EMITI(LOOKBACK(1, 1) - c);
		} else if (c == 0x40 && scc > 0) {
			/* 0x40 p
			 * last duplet, first is p
			 */
			READC(c);
			EMITI(c);
			EMITI(LOOKBACK(1, 1));
		} else if ((c & 0xf0) == 0x40 && c != 0x40 && scc > 0) {
			/* 0100mmmm
			 * repeat last duplet, first is pixel -m
			 */
			c = (c & 0x0f);
			EMITI(LOOKBACKP(c));
			EMITI(LOOKBACK(1, 1));
		} else if (c == 0x50 && scc > 0) {
			/* 0x50 p1 p2
			 * two absolute values
			 */
			READC(c);
			EMITI(c);
			READC(c);
			EMITI(c);
		} else if (c >= 0x51 && c <= 0x57 && scc > 0) {
			/* 01010mmm p
			 * output pixel -m, then p
			 */
			t1 = (c & 0x07);
			EMITI(LOOKBACKP(t1));
			READC(c);
			EMITI(c);
		} else if (c >= 0x59 && c <= 0x5f && scc > 0) {
			/* 01011mmm p
			 * output pixel p, then -m
			 */
			t1 = (c & 0x07);
			READC(c);
			EMITI(c);
			EMITI(LOOKBACKP(t1 + 1)); /* deal with lookback weirdness */
		} else if ((c & 0xf0) == 0x60 && scc > 0) {
			/* 0110xxxx p
			 * output pix p, last duplet 2 +X
			 */
			t1 = (c & 0x0f);
			READC(c);
			EMITI(c);
			EMITI(LOOKBACK(1, 1) + t1);
		} else if ((c & 0xf0) == 0x70 && scc > 0) {
			/* 0111xxxx p
			 * output pix p, last duplet 2 -X
			 */
			t1 = (c & 0x0f);
			READC(c);
			EMITI(c);
			EMITI(LOOKBACK(1, 1) - t1);
		} else if ((c & 0xf0) == 0x80 && scc > 0) {
			/* 1000xxxx
			 * last duplet, +x on 1
			 */
			c = (c & 0x0f);
			EMITI(LOOKBACK(1, 0) + c);
			EMITI(LOOKBACK(1, 1));
		} else if ((c & 0xf0) == 0x90 && scc > 0) {
			/* 1001xxxx p
			 * last duplet 1 + x, pix p
			 */
			EMITI(LOOKBACK(1, 0) + (c & 0x0f));
			READC(c);
			EMITI(c);
		} else if (c == 0xa0 && scc > 0) {
			/* 0xa0 xxxxyyyy
			 * last duplet, +x on 1, +y on 2
			 */
			READC(c);
			t1 = (c & 0xf0) >> 4;
			t2 = (c & 0x0f);
			EMITI(LOOKBACK(1, 0) + t1);
			EMITI(LOOKBACK(1, 1) + t2);
		} else if (c == 0xb0 && scc > 0) {
			/* 0xb0 xxxxyyyy
			 * last duplet, +x on 1, -y on 2
			 */
			READC(c);
			t1 = (c & 0xf0) >> 4;
			t2 = (c & 0x0f);
			EMITI(LOOKBACK(1, 0) + t1);
			EMITI(LOOKBACK(1, 1) - t2);
		} else if ((c & 0xf0) == 0xc0 && scc > 0) {
			/* 1100xxxx
			 * last duplet, -x on 1
			 */
			c = (c & 0x0f);
			EMITI(LOOKBACK(1, 0) - c);
			EMITI(LOOKBACK(1, 1));
		} else if ((c & 0xf0) == 0xd0 && scc > 0) {
			/* 1101xxxx p
			 * last duplet 1 - x, pix p
			 */
			EMITI(LOOKBACK(1, 0) - (c & 0x0f));
			READC(c);
			EMITI(c);
		} else if (c == 0xe0 && scc > 0) {
			/* 0xe0 xxxxyyyy
			 * last duplet, -x on 1, +y on 2
			 */
			READC(c);
			t1 = (c & 0xf0) >> 4;
			t2 = (c & 0x0f);
			EMITI(LOOKBACK(1, 0) - t1);
			EMITI(LOOKBACK(1, 1) + t2);
		} else if (c == 0xf0 && scc > 0) {
			/* 0xfx xxxxyyyy
			 * last duplet, -x on 1, -y on 2
			 */
			READC(c);
			t1 = (c & 0xf0) >> 4;
			t2 = (c & 0x0f);
			EMITI(LOOKBACK(1, 0) - t1);
			EMITI(LOOKBACK(1, 1) - t2);
		} else if ((c & 0xa0) == 0xa0 && scc > 0) {
			/* 1x1xxxmm mmmmmmmm (p)
			 * complicated! n is t1, r is t2
			 * m is ut1
			 */
			if (c >= 0xa4 && c <= 0xab)
				t1 = 2;
			if (c >= 0xac && c <= 0xb7)
				t1 = 3;
			if (c >= 0xb8 && c <= 0xbf)
				t1 = 4;
			if (c >= 0xe4 && c <= 0xeb)
				t1 = 5;
			if (c >= 0xec && c <= 0xf7)
				t1 = 6;
			if (c >= 0xf8 && c <= 0xfb)
				t1 = 7;
			t2 = 1;
			if (c & 0x04)
				t2 = 0;
			if (c & 0x10)
				t2 = 1 - t2;
			
			if (c == 0xfc)
			{
				READC(t3);
				t1 = (t3 & 0xf8) >> 3;
				t1 += 2;
				t2 = 0;
				if (t3 & 0x04)
					t2 = 1;
			}
			
			ut1 = (c & 0x03) << 8;
			if (c == 0xfc)
				ut1 = (t3 & 0x03) << 8;
			READC(t3);
			ut1 |= t3;
			
			st1 = ut1;
			while (st1 > (long)ut1 - (long)(t1 * 2))
			{
				EMITI(LOOKBACKP(st1));
				--st1;
			}
			
			if (t2 == 0)
			{
				--i;
				READC(c);
				EMITI(c);
			}
		} else {
			printf ("Unknown tBMP command: 0x%x\n", c);
			if (scc > 0)
				printf("(is a subcommand)\n");
			break;
		}
		if (scc > 0)
			--scc;
	}
	
	i = 0;

	while (i < bmp->width * bmp->height)
	{
		si = i / bmp->width; /* y */
		ut1 = i % bmp->width; /* x */
		c = decoded[(si * bmp->bytes_per_row) + ut1];
		EMIT(c);
	}

	free(decoded);

	return bmp->data;
}

uint8_t* vaht_bmp_palette(vaht_bmp* bmp)
{
	vaht_bmp_data(bmp);
	return bmp->table;
}

uint8_t* vaht_bmp_indexed_data(vaht_bmp* bmp)
{
	vaht_bmp_data(bmp);
	return bmp->idata;
}

void vaht_bmp_close(vaht_bmp* bmp)
{
	vaht_resource_close(bmp->res);
	if (bmp->data)
		free(bmp->data);
	if (bmp->table)
		free(bmp->table);
	if (bmp->idata)
		free(bmp->idata);
	free(bmp);
}

uint16_t vaht_bmp_width(vaht_bmp* bmp)
{
	return bmp->width;
}

uint16_t vaht_bmp_height(vaht_bmp* bmp)
{
	return bmp->height;
}

uint8_t vaht_bmp_compressed(vaht_bmp* bmp)
{
	if (bmp->format & 0x1)
		return 1;
	return 0;
}

uint8_t vaht_bmp_truecolor(vaht_bmp* bmp)
{
	if (bmp->format & 0x2)
		return 1;
	return 0;
}

