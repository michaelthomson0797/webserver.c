import { getAllPosts } from "@/lib/posts"
import Link from "next/link";

export default function Home() {
  const pages = getAllPosts(["title", "slug"]);
  return (
    <main>
      {pages.map(page => (
        <Link href={`/blog/${page.slug}`} key={page.title}>
          {page.title}
        </Link>
      ))}
    </main>
  )
}


