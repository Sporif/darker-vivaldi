// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <string.h>

#include <string>
#include <utility>

#include "base/logging.h"
#include "base/strings/string_piece.h"
#include "mojo/edk/test/mojo_test_base.h"
#include "mojo/public/c/system/types.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace mojo {
namespace edk {
namespace {

using SharedBufferTest = test::MojoTestBase;

TEST_F(SharedBufferTest, CreateSharedBuffer) {
  const std::string message = "hello";
  MojoHandle h = CreateBuffer(message.size());
  WriteToBuffer(h, 0, message);
  ExpectBufferContents(h, 0, message);
}

TEST_F(SharedBufferTest, DuplicateSharedBuffer) {
  const std::string message = "hello";
  MojoHandle h = CreateBuffer(message.size());
  WriteToBuffer(h, 0, message);

  MojoHandle dupe = DuplicateBuffer(h);
  ExpectBufferContents(dupe, 0, message);
}

TEST_F(SharedBufferTest, PassSharedBufferLocal) {
  const std::string message = "hello";
  MojoHandle h = CreateBuffer(message.size());
  WriteToBuffer(h, 0, message);

  MojoHandle dupe = DuplicateBuffer(h);
  MojoHandle p0, p1;
  CreateMessagePipe(&p0, &p1);

  WriteMessageWithHandles(p0, "...", &dupe, 1);
  EXPECT_EQ("...", ReadMessageWithHandles(p1, &dupe, 1));

  ExpectBufferContents(dupe, 0, message);
}

#if !defined(OS_IOS)

// Reads a single message with a shared buffer handle, maps the buffer, copies
// the message contents into it, then exits.
DEFINE_TEST_CLIENT_TEST_WITH_PIPE(CopyToBufferClient, SharedBufferTest, h) {
  MojoHandle b;
  std::string message = ReadMessageWithHandles(h, &b, 1);
  WriteToBuffer(b, 0, message);

  EXPECT_EQ("quit", ReadMessage(h));
}

#if defined(OS_ANDROID)
// Android multi-process tests are not executing the new process. This is flaky.
#define MAYBE_PassSharedBufferCrossProcess DISABLED_PassSharedBufferCrossProcess
#else
#define MAYBE_PassSharedBufferCrossProcess PassSharedBufferCrossProcess
#endif
TEST_F(SharedBufferTest, MAYBE_PassSharedBufferCrossProcess) {
  const std::string message = "hello";
  MojoHandle b = CreateBuffer(message.size());

  RUN_CHILD_ON_PIPE(CopyToBufferClient, h)
    MojoHandle dupe = DuplicateBuffer(b);
    WriteMessageWithHandles(h, message, &dupe, 1);
    WriteMessage(h, "quit");
  END_CHILD()

  ExpectBufferContents(b, 0, message);
}

// Creates a new buffer, maps it, writes a message contents to it, unmaps it,
// and finally passes it back to the parent.
DEFINE_TEST_CLIENT_TEST_WITH_PIPE(CreateBufferClient, SharedBufferTest, h) {
  std::string message = ReadMessage(h);
  MojoHandle b = CreateBuffer(message.size());
  WriteToBuffer(b, 0, message);
  WriteMessageWithHandles(h, "have a buffer", &b, 1);

  EXPECT_EQ("quit", ReadMessage(h));
}

#if defined(OS_ANDROID)
// Android multi-process tests are not executing the new process. This is flaky.
#define MAYBE_PassSharedBufferFromChild DISABLED_PassSharedBufferFromChild
#else
#define MAYBE_PassSharedBufferFromChild PassSharedBufferFromChild
#endif
TEST_F(SharedBufferTest, MAYBE_PassSharedBufferFromChild) {
  const std::string message = "hello";
  MojoHandle b;
  RUN_CHILD_ON_PIPE(CreateBufferClient, h)
    WriteMessage(h, message);
    ReadMessageWithHandles(h, &b, 1);
    WriteMessage(h, "quit");
  END_CHILD()

  ExpectBufferContents(b, 0, message);
}

DEFINE_TEST_CLIENT_TEST_WITH_PIPE(CreateAndPassBuffer, SharedBufferTest, h) {
  // Receive a pipe handle over the primordial pipe. This will be connected to
  // another child process.
  MojoHandle other_child;
  std::string message = ReadMessageWithHandles(h, &other_child, 1);

  // Create a new shared buffer.
  MojoHandle b = CreateBuffer(message.size());

  // Send a copy of the buffer to the parent and the other child.
  MojoHandle dupe = DuplicateBuffer(b);
  WriteMessageWithHandles(h, "", &b, 1);
  WriteMessageWithHandles(other_child, "", &dupe, 1);

  EXPECT_EQ("quit", ReadMessage(h));
  WriteMessage(h, "ok");
}

DEFINE_TEST_CLIENT_TEST_WITH_PIPE(ReceiveAndEditBuffer, SharedBufferTest, h) {
  // Receive a pipe handle over the primordial pipe. This will be connected to
  // another child process (running CreateAndPassBuffer).
  MojoHandle other_child;
  std::string message = ReadMessageWithHandles(h, &other_child, 1);

  // Receive a shared buffer from the other child.
  MojoHandle b;
  ReadMessageWithHandles(other_child, &b, 1);

  // Write the message from the parent into the buffer and exit.
  WriteToBuffer(b, 0, message);
  EXPECT_EQ(MOJO_RESULT_OK, MojoClose(b));
  EXPECT_EQ("quit", ReadMessage(h));
  WriteMessage(h, "ok");
}

#if defined(OS_ANDROID)
// Android multi-process tests are not executing the new process. This is flaky.
#define MAYBE_PassSharedBufferFromChildToChild \
    DISABLED_PassSharedBufferFromChildToChild
#else
#define MAYBE_PassSharedBufferFromChildToChild PassSharedBufferFromChildToChild
#endif
TEST_F(SharedBufferTest, MAYBE_PassSharedBufferFromChildToChild) {
  const std::string message = "hello";
  MojoHandle p0, p1;
  CreateMessagePipe(&p0, &p1);

  MojoHandle b;
  RUN_CHILD_ON_PIPE(CreateAndPassBuffer, h0)
    RUN_CHILD_ON_PIPE(ReceiveAndEditBuffer, h1)
      // Send one end of the pipe to each child. The first child will create
      // and pass a buffer to the second child and back to us. The second child
      // will write our message into the buffer.
      WriteMessageWithHandles(h0, message, &p0, 1);
      WriteMessageWithHandles(h1, message, &p1, 1);

      // Receive the buffer back from the first child.
      ReadMessageWithHandles(h0, &b, 1);

      WriteMessage(h1, "quit");
      EXPECT_EQ("ok", ReadMessage(h1));
    END_CHILD()
    WriteMessage(h0, "quit");
    EXPECT_EQ("ok", ReadMessage(h0));
  END_CHILD()

  // The second child should have written this message.
  ExpectBufferContents(b, 0, message);
}

DEFINE_TEST_CLIENT_TEST_WITH_PIPE(CreateAndPassBufferParent, SharedBufferTest,
                                  parent) {
  RUN_CHILD_ON_PIPE(CreateAndPassBuffer, child)
    // Read a pipe from the parent and forward it to our child.
    MojoHandle pipe;
    std::string message = ReadMessageWithHandles(parent, &pipe, 1);

    WriteMessageWithHandles(child, message, &pipe, 1);

    // Read a buffer handle from the child and pass it back to the parent.
    MojoHandle buffer;
    EXPECT_EQ("", ReadMessageWithHandles(child, &buffer, 1));
    WriteMessageWithHandles(parent, "", &buffer, 1);

    EXPECT_EQ("quit", ReadMessage(parent));
    WriteMessage(child, "quit");
    EXPECT_EQ("ok", ReadMessage(child));
    WriteMessage(parent, "ok");
  END_CHILD()
}

DEFINE_TEST_CLIENT_TEST_WITH_PIPE(ReceiveAndEditBufferParent, SharedBufferTest,
                                  parent) {
  RUN_CHILD_ON_PIPE(ReceiveAndEditBuffer, child)
    // Read a pipe from the parent and forward it to our child.
    MojoHandle pipe;
    std::string message = ReadMessageWithHandles(parent, &pipe, 1);
    WriteMessageWithHandles(child, message, &pipe, 1);

    EXPECT_EQ("quit", ReadMessage(parent));
    WriteMessage(child, "quit");
    EXPECT_EQ("ok", ReadMessage(child));
    WriteMessage(parent, "ok");
  END_CHILD()
}

#if defined(OS_ANDROID)
// Android multi-process tests are not executing the new process. This is flaky.
#define MAYBE_PassHandleBetweenCousins DISABLED_PassHandleBetweenCousins
#else
#define MAYBE_PassHandleBetweenCousins PassHandleBetweenCousins
#endif
TEST_F(SharedBufferTest, MAYBE_PassHandleBetweenCousins) {
  const std::string message = "hello";
  MojoHandle p0, p1;
  CreateMessagePipe(&p0, &p1);

  // Spawn two children who will each spawn their own child. Make sure the
  // grandchildren (cousins to each other) can pass platform handles.
  MojoHandle b;
  RUN_CHILD_ON_PIPE(CreateAndPassBufferParent, child1)
    RUN_CHILD_ON_PIPE(ReceiveAndEditBufferParent, child2)
      MojoHandle pipe[2];
      CreateMessagePipe(&pipe[0], &pipe[1]);

      WriteMessageWithHandles(child1, message, &pipe[0], 1);
      WriteMessageWithHandles(child2, message, &pipe[1], 1);

      // Receive the buffer back from the first child.
      ReadMessageWithHandles(child1, &b, 1);

      WriteMessage(child2, "quit");
      EXPECT_EQ("ok", ReadMessage(child2));
    END_CHILD()
    WriteMessage(child1, "quit");
    EXPECT_EQ("ok", ReadMessage(child1));
  END_CHILD()

  // The second grandchild should have written this message.
  ExpectBufferContents(b, 0, message);
}

#endif  // !defined(OS_IOS)

}  // namespace
}  // namespace edk
}  // namespace mojo