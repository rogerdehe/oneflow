"""
Copyright 2020 The OneFlow Authors. All rights reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
"""
import numpy as np


def sigmoid(x):
    y = 1 / (1 + np.exp(-x))
    return y


def sigmoid_grad(y, dy):
    return y * (1 - y) * dy


def forward(args):
    print("forward args", args)
    return sigmoid(*args)


def backward(args):
    print("backward args", args)
    return sigmoid_grad(*args)