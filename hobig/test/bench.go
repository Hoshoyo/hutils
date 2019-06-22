package main

import (
	"fmt"
	"math/big"
	"os"
	"strings"
	"time"
)

func testMultiplyBenchmark() {
	v := new(big.Int)
	d := new(big.Int)

	start := time.Now()

	for k := 0; k < 10000; k++ {
		v, _ = v.SetString("9812029183092109833851794719827489214120981798579728720418092047123721837128371298739812", 10)
		d, _ = d.SetString("3230291580290704972938712983791287012085289572097481203112093812908390218093890218903120", 10)
		for j := 0; j < 16; j++ {
			v = v.Mul(v, d)
		}
	}

	fmt.Println(v)

	//fmt.Println(float64(time.Now().Sub(start).Nanoseconds()) / 1000000.0)
	fmt.Println(time.Now().Sub(start))
}

func testAdditionBenchmark() {
	v := new(big.Int)
	d := new(big.Int)
	v, _ = v.SetString("9812029183092109833851794719827489214120981798579728720418092047123721837128371298739812", 10)
	d, _ = d.SetString("3230291580290704972938712983791287012085289572097481203112093812908390218093890218903120", 10)

	start := time.Now()

	for j := 0; j < 100; j++ {
		for i := 0; i < 1000000; i++ {
			v.Add(v, d)
		}
	}

	fmt.Println(float64(time.Now().Sub(start).Nanoseconds()) / 1000000.0)
}

func testSubtractionBenchmark() {
	v := new(big.Int)
	d := new(big.Int)
	v, _ = v.SetString("9812029183092109833851794719827489214120981798579728720418092047123721837128371298739812", 10)
	d, _ = d.SetString("3230291580290704972938712983791287012085289572097481203112093812908390218093890218903120", 10)

	start := time.Now()

	for j := 0; j < 100; j++ {
		for i := 0; i < 1000000; i++ {
			v.Sub(v, d)
		}
	}

	fmt.Println(float64(time.Now().Sub(start).Nanoseconds()) / 1000000.0)
}

func main() {
	if len(os.Args) < 2 {
		names := strings.Split(os.Args[0], "/")
		fmt.Printf("usage: %s <benchmark_name>\n", names[len(names)-1])
		os.Exit(1)
	}

	switch os.Args[1] {
	case "sum":
		testAdditionBenchmark()
	case "sub":
		testSubtractionBenchmark()
	case "mul":
		testMultiplyBenchmark()
	default:
		fmt.Println("Invalid benchmark name")
		os.Exit(1)
	}
}
